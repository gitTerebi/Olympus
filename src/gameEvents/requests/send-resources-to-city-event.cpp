#include "send-resources-to-city-event.h"
#include "requested-resources-type.h"
#include "request-state.h"

#include "engine/game-board.h"
#include "elanguage.h"
#include "estringhelpers.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "emessages.h"
#include "engine/ecityrequest.h"
#include "fileIO/esavearchive.h"

#include <cstdio>

namespace
{
    int sNextReceiveRequestId = 1;

    const std::vector<RequestState>& fulfillRequestStates()
    {
        static const std::vector<RequestState> states = {
            {0, 12},
            {6, 6},
            {12, 12},
            {18, 6},
            {24, 0},
        };
        return states;
    }

    const RequestState& fulfillRequestState(const int step)
    {
        return requestState(fulfillRequestStates(), step);
    }

    bool fulfillRequestTerminalState(const int step)
    {
        return requestTerminalState(fulfillRequestStates(), step);
    }

    RequestedResourcesResult requestedResourcesResultForStep(const int step)
    {
        return step > 2 ? RequestedResourcesResult::tooLate : RequestedResourcesResult::comply;
    }

    bool canPostponeRequestStep(const int requestStep)
    {
        return !fulfillRequestTerminalState(requestStep);
    }

    int displayMonthsForRequestStep(const int requestStep)
    {
        return fulfillRequestState(requestStep).fComplyMonths;
    }
}

SendResourcesToCityEvent::SendResourcesToCityEvent(
    const eCityId cid,
    const eGameEventBranch branch,
    GameBoard &board) : eGameEvent(cid, eGameEventType::sendResourcesToCity, branch, board),
                         eCityEventValue(board)
{
    const auto e1 = eLanguage::text("early");
    mEarlyTrigger = e::make_shared<eEventTrigger>(cid, e1, board);
    const auto e2 = eLanguage::text("comply");
    mComplyTrigger = e::make_shared<eEventTrigger>(cid, e2, board);
    const auto e3 = eLanguage::text("too_late");
    mTooLateTrigger = e::make_shared<eEventTrigger>(cid, e3, board);
    const auto e4 = eLanguage::text("refuse");
    mRefuseTrigger = e::make_shared<eEventTrigger>(cid, e4, board);

    addTrigger(mEarlyTrigger);
    addTrigger(mComplyTrigger);
    addTrigger(mTooLateTrigger);
    addTrigger(mRefuseTrigger);
}

SendResourcesToCityEvent::~SendResourcesToCityEvent()
{
    const auto board = gameBoard();
    if (board && isMainEvent())
        board->removeCityRequest(this);
}

void SendResourcesToCityEvent::trigger()
{
    const auto board = gameBoard();
    if (!board)
        return;

    if (isMainEvent() && !startRequest(*board))
        return;
    if (!mCity)
        return;

    auto ed = createEventData(*board);
    if (mRequestFinished)
        showRequestFinished(*board, ed);
    else
    {
        addRequestToSidePanel(*board);
        showRequestPopup(*board, ed);
    }
}

bool SendResourcesToCityEvent::startRequest(GameBoard &board)
{
    if (isActiveCityRequest() &&
        mRequestDate == eDate(1, eMonth::january, 1))
    {
        mRequestDate = board.date();
        auto deadline = board.date();
        deadline.nextMonths(warningMonths());
        mRequestDeadline = deadline;
        board.addCityRequest(mainEvent<SendResourcesToCityEvent>());
        return true;
    }
    if (!isActiveCityRequest())
        return initializeRequest(board);
    return startQueuedRequest(board);
}

bool SendResourcesToCityEvent::startQueuedRequest(GameBoard &board)
{
    const auto request = e::make_shared<SendResourcesToCityEvent>(
        cityId(), eGameEventBranch::root, board);
    request->setWarningMonths(warningMonths());
    request->setRequestType(mRequestType);
    request->initializeDate(board.date());
    board.addRootGameEvent(request);
    request->trigger();
    request->setRepeat(0);
    return false;
}

bool SendResourcesToCityEvent::initializeRequest(GameBoard &board)
{
    mRequestStep = 0;
    mRequestId = sNextReceiveRequestId++;
    mRequestFinished = false;
    mRequestResult = RequestedResourcesResult::comply;
    mRequestDate = board.date();
    // Side panel uses this deadline; reminder popups must not move it.
    auto deadline = board.date();
    deadline.nextMonths(fulfillRequestState(0).fComplyMonths);
    mRequestDeadline = deadline;
    mComplyStep = 0;
    mPostponed = false;
    mComplyStartDate = board.date();
    chooseCity();
    if (!mCity)
        return false;
    chooseType();
    chooseCount();
    board.addCityRequest(mainEvent<SendResourcesToCityEvent>());
    return true;
}

eEventData SendResourcesToCityEvent::createEventData(GameBoard &board) const
{
    const auto pid = mRequestType == RequestedResourcesType::tribute ? board.personPlayer() : playerId();
    eEventData ed(pid);
    ed.fCity = mCity;
    ed.fResourceType = mResource;
    ed.fResourceCount = mCount;
    ed.fTime = displayMonthsForRequestStep(mRequestStep);
    ed.fGod = mGod;
    ed.fEventRuntimeId = runtimeId();
    return ed;
}

void SendResourcesToCityEvent::showRequestFinished(
    GameBoard &board, eEventData &ed)
{
    if (!mCity)
        return;
    ed.fType = eMessageEventType::resourceGranted;
    const auto *msgs = requestedResourcesMessages(mRequestType, *mCity);
    if (mRequestResult == RequestedResourcesResult::refuse)
    {
        const auto event = requestedResourcesFinishEvent(
            mRequestType, *mCity, RequestedResourcesFinish::refuse);
        board.event(event, ed);
        const auto &reason = msgs->fRefuseReason;
        const auto me = mainEvent<SendResourcesToCityEvent>();
        me->finished(*me->mRefuseTrigger, reason);
        return;
    }

    if (mRequestResult == RequestedResourcesResult::tooLate)
    {
        const auto event = requestedResourcesFinishEvent(
            mRequestType, *mCity, RequestedResourcesFinish::tooLate);
        board.event(event, ed);
        const auto &reason = msgs->fTooLateReason;
        const auto me = mainEvent<SendResourcesToCityEvent>();
        me->finished(*me->mTooLateTrigger, reason);
        return;
    }

    const auto event = requestedResourcesFinishEvent(
        mRequestType, *mCity, RequestedResourcesFinish::comply);
    board.event(event, ed);
    const auto &reason = msgs->fComplyReason;
    const auto me = mainEvent<SendResourcesToCityEvent>();
    me->finished(*me->mComplyTrigger, reason);
}

void SendResourcesToCityEvent::showRequestPopup(GameBoard &board, eEventData &ed)
{
    fillEventDataActions(ed);

    ed.fType = eMessageEventType::generalRequestGranted;
    const auto uiEvent = requestedResourcesStepEvent(
        mRequestType, *mCity, mRequestStep);
    board.event(uiEvent, ed);
}

void SendResourcesToCityEvent::fillEventDataActions(eEventData &ed)
{
    const auto board = gameBoard();
    const auto request = mainEvent<SendResourcesToCityEvent>();
    if (!board || !request || !request->isActiveCityRequest())
        return;

    ed.fType = eMessageEventType::generalRequestGranted;
    ed.fEventRuntimeId = request->runtimeId();
    ed.fCity = request->mCity;
    ed.fResourceType = request->mResource;
    ed.fResourceCount = request->mCount;
    ed.fTime = displayMonthsForRequestStep(request->mRequestStep);
    ed.fGod = request->mGod;
    ed.fCloseResponse = -1;
    ed.fPrimaryResponse = -1;
    ed.fCityNames.clear();
    ed.fCityConditionalResponses.clear();
    ed.fCSpaceCount.clear();
    ed.fSecondaryResponse = -1;
    ed.fTertiaryResponse = -1;

    request->addFulfillButton(*board, ed);
    request->addPostponeButton(*board, ed);
    request->addRefuseButton(*board, ed);
}

void SendResourcesToCityEvent::addFulfillButton(GameBoard &board, eEventData &ed)
{
    const auto pid = mRequestType == RequestedResourcesType::tribute ? board.personPlayer() : playerId();
    addRequestDispatchResponses(board, ed, pid, mResource, mCount,
                                static_cast<int>(eResponse::dispatch));
}

void SendResourcesToCityEvent::addRequestToSidePanel(GameBoard &board)
{
    const auto request = mainEvent<SendResourcesToCityEvent>();
    if (request && request->isActiveCityRequest())
    {
        board.addCityRequest(request);
    }
}

void SendResourcesToCityEvent::addPostponeButton(
    GameBoard &board, eEventData &ed)
{
    const auto request = mainEvent<SendResourcesToCityEvent>();
    if (canPostponeRequestStep(mRequestStep))
    {
        ed.fSecondaryResponse = static_cast<int>(eResponse::postpone);
    }
}

void SendResourcesToCityEvent::addRefuseButton(GameBoard &board, eEventData &ed)
{
    ed.fTertiaryResponse = static_cast<int>(eResponse::refuse);
}

void SendResourcesToCityEvent::respond(const int response, const eCityId city)
{
    switch (static_cast<eResponse>(response))
    {
    case eResponse::dispatch:
        if (city == eCityId::neutralAggresive)
        {
            const auto board = gameBoard();
            if (!board)
                return;
            const auto pid = mRequestType == RequestedResourcesType::tribute ? board->personPlayer() : playerId();
            const auto cids = board->playerCitiesOnBoard(pid);
            if (!cids.empty())
                dispatch(cids[0]);
        }
        else
        {
            dispatch(city);
        }
        break;
    case eResponse::postpone:
        postpone();
        break;
    case eResponse::refuse:
        if (const auto request = mainEvent<SendResourcesToCityEvent>())
            request->finish(RequestedResourcesResult::refuse);
        break;
    }
}

void SendResourcesToCityEvent::postpone()
{
    const auto request = mainEvent<SendResourcesToCityEvent>();
    if (!request)
        return;

    request->mPostponed = true;
}

int SendResourcesToCityEvent::complyMonths() const
{
    return fulfillRequestState(mComplyStep).fComplyMonths;
}

std::string SendResourcesToCityEvent::longName() const
{
    auto tmpl = eLanguage::text("receive_request_long_name");
    eCountEventValue::longNameReplaceCount("%1", tmpl);
    eResourceEventValue::longNameReplaceResource("%2", tmpl);
    return tmpl;
}

void SendResourcesToCityEvent::serializeFields(eSaveArchive &ar)
{
    eGameEvent::serializeFields(ar);
    eResourceEventValue::serialize(ar);
    eCountEventValue::serialize(ar);
    eCityEventValue::serialize(ar, *gameBoard());
    eGodEventValue::serialize(ar);
    ar.field("requestType", mRequestType, RequestedResourcesType::general);
    ar.field("requestResult", mRequestResult, RequestedResourcesResult::comply);
    ar.field("requestFinished", mRequestFinished, false);
    ar.field("requestStep", mRequestStep, 0);
    ar.field("requestId", mRequestId, 0);
    ar.field("complyStep", mComplyStep, 0);
    ar.field("postponed", mPostponed, false);
    ar.dateField("requestDate", mRequestDate);
    ar.dateField("requestDeadline", mRequestDeadline);
    ar.dateField("complyStartDate", mComplyStartDate);
    if (ar.reading() && mPostponed && mRequestStep == 0)
    {
        mRequestStep = 2;
    }
}

eCityRequest SendResourcesToCityEvent::cityRequest() const
{
    eCityRequest request;
    request.fCity = mCity;
    request.fType = mResource;
    request.fCount = mCount;
    return request;
}

void SendResourcesToCityEvent::dispatch(const eCityId cid)
{
    const auto board = gameBoard();
    if (!board)
        return;
    board->takeResource(cid, mResource, mCount);
    finish(requestedResourcesResultForStep(mRequestStep));
}

void SendResourcesToCityEvent::finish(const RequestedResourcesResult result)
{
    const auto board = gameBoard();
    if (!board)
        return;
    clearConsequences();
    const auto request = mainEvent<SendResourcesToCityEvent>();
    board->removeCityRequest(request);
    mRequestFinished = true;
    mRequestResult = result;

    if (mCity && result == RequestedResourcesResult::refuse)
        board->changeCityAttitude(mCity, -10, playerId());
    else if (mCity && result == RequestedResourcesResult::tooLate)
        board->changeCityAttitude(mCity, 5, playerId());
    else if (mCity)
        board->changeCityAttitude(mCity, 10, playerId());

    if (request)
    {
        request->mRequestFinished = true;
        request->mRequestResult = result;
    }

    const auto pid = mRequestType == RequestedResourcesType::tribute ? board->personPlayer() : playerId();
    eEventData ed(pid);
    ed.fCity = mCity;
    ed.fResourceType = mResource;
    ed.fResourceCount = mCount;
    ed.fGod = mGod;
    showRequestFinished(*board, ed);
}

void SendResourcesToCityEvent::finished(eEventTrigger &t, const eReason &r)
{
    const auto board = gameBoard();
    if (!board)
        return;
    const auto date = board->date();
    auto rFull = r.fFull;
    const auto amount = std::to_string(mCount);
    eStringHelpers::replaceAll(rFull, "[amount]", amount);
    const auto item = eResourceTypeHelpers::typeLongName(mResource);
    eStringHelpers::replaceAll(rFull, "[item]", item);
    t.trigger(*this, date, rFull);
}

eDate SendResourcesToCityEvent::complyDate() const
{
    if (mRequestDeadline != eDate(1, eMonth::january, 1))
        return mRequestDeadline;

    auto date = mRequestDate;
    const auto board = gameBoard();
    if (date == eDate(1, eMonth::january, 1) && board)
        date = board->date();
    date.nextMonths(warningMonths());
    return date;
}

bool SendResourcesToCityEvent::isOverdue(const eDate &currentDate) const
{
    return currentDate > complyDate();
}

void SendResourcesToCityEvent::advanceIfNeeded(const eDate &currentDate)
{
    const auto board = gameBoard();
    if (!board || !isMainEvent() || !isActiveCityRequest())
        return;
    if (mRequestDate == eDate(1, eMonth::january, 1))
        return;
    const auto& states = fulfillRequestStates();
    const int nextStep = mRequestStep + 1;
    if (nextStep >= static_cast<int>(states.size()))
        return;
    auto nextDate = mRequestDate;
    nextDate.nextMonths(states[nextStep].fPopupMonth);
    if (currentDate < nextDate)
        return;
    advanceToNextStep(*board);
}

void SendResourcesToCityEvent::advanceToNextStep(GameBoard &board)
{
    const auto& states = fulfillRequestStates();
    const int nextStep = mRequestStep + 1;
    if (nextStep >= static_cast<int>(states.size()) ||
        fulfillRequestTerminalState(nextStep))
    {
        finish(RequestedResourcesResult::refuse);
        return;
    }

    clearConsequences();
    mPostponed = false;
    mRequestStep = nextStep;
    mComplyStep = nextStep;
    mComplyStartDate = board.date();
    mRequestDeadline = board.date();
    mRequestDeadline.nextMonths(complyMonths());

    auto ed = createEventData(board);
    addRequestToSidePanel(board);
    showRequestPopup(board, ed);
}

bool SendResourcesToCityEvent::isPostponed() const
{
    const int overdueStep = 2;
    return mRequestStep >= overdueStep;
}

bool SendResourcesToCityEvent::finished() const
{
    return eGameEvent::finished() && !isActiveCityRequest();
}

bool SendResourcesToCityEvent::isActiveCityRequest() const
{
    return !mRequestFinished && mCity;
}

std::string SendResourcesToCityEvent::overdueStatusText(const eDate &currentDate) const
{
    const auto request = const_cast<SendResourcesToCityEvent *>(this)
                             ->mainEvent<SendResourcesToCityEvent>();
    const auto state = request ? request : this;
    return requestCountdownText(state->complyMonths(),
                                state->mComplyStartDate,
                                currentDate);
}

std::string SendResourcesToCityEvent::requestInfo(int stock, const eDate &currentDate) const
{
    const bool overdue = isOverdue(currentDate);
    const int daysDiff = overdue ? currentDate - complyDate() : complyDate() - currentDate;
    const int remainingMonths = daysDiff / 31;
    return resourceRequestInfo(mResource, mCount, stock, overdue,
                               remainingMonths);
}

std::string SendResourcesToCityEvent::dispatchText(int stock, const eDate &currentDate) const
{
    return resourceDispatchText(requestInfo(stock, currentDate));
}
