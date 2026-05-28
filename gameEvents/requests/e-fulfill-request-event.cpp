#include "e-fulfill-request-event.h"
#include "e-receive-request-type.h"

#include "engine/e-game-board.h"
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

    struct eFulfillRequestState
    {
        int fPopupMonth;
        int fComplyMonths;
    };

    const std::vector<eFulfillRequestState> &fulfillRequestStates()
    {
        static const std::vector<eFulfillRequestState> states = {
            {0, 12},  // Step 0: Initial request, popup at month 0, comply within 12 months
            {6,  6}, // Step 1: Reminder, popup at month 2, comply within 2 months
            {12, 12}, // Step 2: Overdue, popup at month 12, comply within 12 months
            {18, 6},  // Step 3: Warning, popup at month 18, comply within 6 months
            {24, 0},  // Step 4: Too late, popup at month 24, no compliance possible
        };
        return states;
    }

    const eFulfillRequestState &fulfillRequestState(const int step)
    {
        const auto &states = fulfillRequestStates();
        if (step >= 0 && step < static_cast<int>(states.size()))
            return states[step];
        return states.front();
    }

    bool fulfillRequestTerminalState(const int step)
    {
        // Terminal state is "too late" - identified by 0 comply months
        return fulfillRequestState(step).fComplyMonths == 0;
    }

    eReceiveRequestResult fulfillRequestResultForStep(const int step)
    {
        return step > 2 ? eReceiveRequestResult::tooLate : eReceiveRequestResult::comply;
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

eFulfillRequestEvent::eFulfillRequestEvent(
    const eCityId cid,
    const eGameEventBranch branch,
    eGameBoard &board) : eGameEvent(cid, eGameEventType::receiveRequest, branch, board),
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

eFulfillRequestEvent::~eFulfillRequestEvent()
{
    const auto board = gameBoard();
    if (board && isMainEvent())
        board->removeCityRequest(this);
}

void eFulfillRequestEvent::trigger()
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

bool eFulfillRequestEvent::startRequest(eGameBoard &board)
{
    if (isActiveCityRequest() &&
        mRequestDate == eDate(1, eMonth::january, 1))
    {
        mRequestDate = board.date();
        auto deadline = board.date();
        deadline.nextMonths(warningMonths());
        mRequestDeadline = deadline;
        board.addCityRequest(mainEvent<eFulfillRequestEvent>());
        return true;
    }
    if (!isActiveCityRequest())
        return initializeRequest(board);
    return startQueuedRequest(board);
}

bool eFulfillRequestEvent::startQueuedRequest(eGameBoard &board)
{
    const auto request = e::make_shared<eFulfillRequestEvent>(
        cityId(), eGameEventBranch::root, board);
    request->setWarningMonths(warningMonths());
    request->setRequestType(mRequestType);
    request->initializeDate(board.date());
    board.addRootGameEvent(request);
    request->trigger();
    request->setRepeat(0);
    return false;
}

bool eFulfillRequestEvent::initializeRequest(eGameBoard &board)
{
    mRequestStep = 0;
    mRequestId = sNextReceiveRequestId++;
    mRequestFinished = false;
    mRequestResult = eReceiveRequestResult::comply;
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
    board.addCityRequest(mainEvent<eFulfillRequestEvent>());
    return true;
}

eEventData eFulfillRequestEvent::createEventData(eGameBoard &board) const
{
    const auto pid = mRequestType == eReceiveRequestType::tribute ? board.personPlayer() : playerId();
    eEventData ed(pid);
    ed.fCity = mCity;
    ed.fResourceType = mResource;
    ed.fResourceCount = mCount;
    ed.fTime = displayMonthsForRequestStep(mRequestStep);
    ed.fGod = mGod;
    ed.fEventRuntimeId = runtimeId();
    return ed;
}

void eFulfillRequestEvent::showRequestFinished(
    eGameBoard &board, eEventData &ed)
{
    if (!mCity)
        return;
    ed.fType = eMessageEventType::resourceGranted;
    const auto *msgs = receiveRequestMessages(mRequestType, *mCity);
    if (mRequestResult == eReceiveRequestResult::refuse)
    {
        const auto event = receiveRequestFinishEvent(
            mRequestType, *mCity, eReceiveRequestFinish::refuse);
        board.event(event, ed);
        const auto &reason = msgs->fRefuseReason;
        const auto me = mainEvent<eFulfillRequestEvent>();
        me->finished(*me->mRefuseTrigger, reason);
        return;
    }

    if (mRequestResult == eReceiveRequestResult::tooLate)
    {
        const auto event = receiveRequestFinishEvent(
            mRequestType, *mCity, eReceiveRequestFinish::tooLate);
        board.event(event, ed);
        const auto &reason = msgs->fTooLateReason;
        const auto me = mainEvent<eFulfillRequestEvent>();
        me->finished(*me->mTooLateTrigger, reason);
        return;
    }

    const auto event = receiveRequestFinishEvent(
        mRequestType, *mCity, eReceiveRequestFinish::comply);
    board.event(event, ed);
    const auto &reason = msgs->fComplyReason;
    const auto me = mainEvent<eFulfillRequestEvent>();
    me->finished(*me->mComplyTrigger, reason);
}

void eFulfillRequestEvent::showRequestPopup(eGameBoard &board, eEventData &ed)
{
    addFulfillButton(board, ed);
    addPostponeButton(board, ed);
    addRefuseButton(board, ed);

    ed.fType = eMessageEventType::generalRequestGranted;
    const auto uiEvent = receiveRequestStepEvent(
        mRequestType, *mCity, mRequestStep);
    board.event(uiEvent, ed);
}

void eFulfillRequestEvent::addFulfillButton(eGameBoard &board, eEventData &ed)
{
    if (mResource == eResourceType::drachmas)
        addDrachmasFulfillButton(board, ed);
    else
        addResourceFulfillButtons(board, ed);
}

void eFulfillRequestEvent::addDrachmasFulfillButton(
    eGameBoard &board, eEventData &ed)
{
    const auto pid = mRequestType == eReceiveRequestType::tribute ? board.personPlayer() : playerId();
    const auto cids = board.playerCitiesOnBoard(pid);
    const int avCount = board.drachmas(pid);
    if (avCount >= mCount && !cids.empty())
    {
        const auto cid = cids[0];
        ed.fPrimaryResponse = static_cast<int>(eResponse::dispatch);
    }
}

void eFulfillRequestEvent::addResourceFulfillButtons(
    eGameBoard &board, eEventData &ed)
{
    const auto pid = mRequestType == eReceiveRequestType::tribute ? board.personPlayer() : playerId();
    const auto cids = board.playerCitiesOnBoard(pid);
    for (const auto cid : cids)
    {
        const int avCount = board.resourceCount(cid, mResource);
        ed.fCityNames[cid] = board.cityName(cid);
        ed.fCSpaceCount[cid] = avCount;

        if (avCount >= mCount)
        {
            ed.fCityConditionalResponses[cid] = static_cast<int>(eResponse::dispatch);
        }
    }
}

void eFulfillRequestEvent::addRequestToSidePanel(eGameBoard &board)
{
    const auto request = mainEvent<eFulfillRequestEvent>();
    if (request && request->isActiveCityRequest())
    {
        board.addCityRequest(request);
    }
}

void eFulfillRequestEvent::addPostponeButton(
    eGameBoard &board, eEventData &ed)
{
    const auto request = mainEvent<eFulfillRequestEvent>();
    if (canPostponeRequestStep(mRequestStep))
    {
        ed.fSecondaryResponse = static_cast<int>(eResponse::postpone);
    }
}

void eFulfillRequestEvent::addRefuseButton(eGameBoard &board, eEventData &ed)
{
    ed.fTertiaryResponse = static_cast<int>(eResponse::refuse);
}

void eFulfillRequestEvent::respond(const int response, const eCityId city)
{
    switch (static_cast<eResponse>(response))
    {
    case eResponse::dispatch:
        if (city == eCityId::neutralAggresive)
        {
            const auto board = gameBoard();
            if (!board)
                return;
            const auto pid = mRequestType == eReceiveRequestType::tribute ? board->personPlayer() : playerId();
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
        if (const auto request = mainEvent<eFulfillRequestEvent>())
            request->finish(eReceiveRequestResult::refuse);
        break;
    }
}

void eFulfillRequestEvent::postpone()
{
    const auto request = mainEvent<eFulfillRequestEvent>();
    if (!request)
        return;

    request->mPostponed = true;
}

int eFulfillRequestEvent::complyMonths() const
{
    return fulfillRequestState(mComplyStep).fComplyMonths;
}

std::string eFulfillRequestEvent::longName() const
{
    auto tmpl = eLanguage::text("receive_request_long_name");
    eCountEventValue::longNameReplaceCount("%1", tmpl);
    eResourceEventValue::longNameReplaceResource("%2", tmpl);
    return tmpl;
}

void eFulfillRequestEvent::serializeFields(eSaveArchive &ar)
{
    eGameEvent::serializeFields(ar);
    eResourceEventValue::serialize(ar);
    eCountEventValue::serialize(ar);
    eCityEventValue::serialize(ar, *gameBoard());
    eGodEventValue::serialize(ar);
    ar.field("requestType", mRequestType, eReceiveRequestType::general);
    ar.field("requestResult", mRequestResult, eReceiveRequestResult::comply);
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

eCityRequest eFulfillRequestEvent::cityRequest() const
{
    eCityRequest request;
    request.fCity = mCity;
    request.fType = mResource;
    request.fCount = mCount;
    return request;
}

void eFulfillRequestEvent::dispatch(const eCityId cid)
{
    const auto board = gameBoard();
    if (!board)
        return;
    board->takeResource(cid, mResource, mCount);
    finish(fulfillRequestResultForStep(mRequestStep));
}

void eFulfillRequestEvent::finish(const eReceiveRequestResult result)
{
    const auto board = gameBoard();
    if (!board)
        return;
    clearConsequences();
    const auto request = mainEvent<eFulfillRequestEvent>();
    board->removeCityRequest(request);
    mRequestFinished = true;
    mRequestResult = result;

    if (mCity && result == eReceiveRequestResult::refuse)
        board->changeCityAttitude(mCity, -10, playerId());
    else if (mCity && result == eReceiveRequestResult::tooLate)
        board->changeCityAttitude(mCity, 5, playerId());
    else if (mCity)
        board->changeCityAttitude(mCity, 10, playerId());

    if (request)
    {
        request->mRequestFinished = true;
        request->mRequestResult = result;
    }

    const auto pid = mRequestType == eReceiveRequestType::tribute ? board->personPlayer() : playerId();
    eEventData ed(pid);
    ed.fCity = mCity;
    ed.fResourceType = mResource;
    ed.fResourceCount = mCount;
    ed.fGod = mGod;
    showRequestFinished(*board, ed);
}

void eFulfillRequestEvent::finished(eEventTrigger &t, const eReason &r)
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

eDate eFulfillRequestEvent::complyDate() const
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

bool eFulfillRequestEvent::isOverdue(const eDate &currentDate) const
{
    return currentDate > complyDate();
}

void eFulfillRequestEvent::advanceIfNeeded(const eDate &currentDate)
{
    const auto board = gameBoard();
    if (!board || !isMainEvent() || !isActiveCityRequest())
        return;
    if (mRequestDate == eDate(1, eMonth::january, 1))
        return;
    const auto &states = fulfillRequestStates();
    const int nextStep = mRequestStep + 1;
    if (nextStep >= static_cast<int>(states.size()))
        return;
    auto nextDate = mRequestDate;
    nextDate.nextMonths(states[nextStep].fPopupMonth);
    if (currentDate < nextDate)
        return;
    advanceToNextStep(*board);
}

void eFulfillRequestEvent::advanceToNextStep(eGameBoard &board)
{
    const auto &states = fulfillRequestStates();
    const int nextStep = mRequestStep + 1;
    if (nextStep >= static_cast<int>(states.size()) ||
        fulfillRequestTerminalState(nextStep))
    {
        finish(eReceiveRequestResult::refuse);
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

bool eFulfillRequestEvent::isPostponed() const
{
    const int overdueStep = 2;
    return mRequestStep >= overdueStep;
}

bool eFulfillRequestEvent::finished() const
{
    return eGameEvent::finished() && !isActiveCityRequest();
}

bool eFulfillRequestEvent::isActiveCityRequest() const
{
    return !mRequestFinished && mCity;
}

int eFulfillRequestEvent::remainingMonths(const eDate &deadline, const eDate &current) const
{
    const int daysDiff = deadline - current;
    return (daysDiff + 30) / 31;
}

std::string eFulfillRequestEvent::overdueStatusText(const eDate &currentDate) const
{
    const auto request = const_cast<eFulfillRequestEvent *>(this)
                             ->mainEvent<eFulfillRequestEvent>();
    const auto state = request ? request : this;
    // Sidebar counts down from the current step's comply window.
    const int comply = state->complyMonths();
    const int elapsedMonths =
        state->mComplyStartDate == eDate(1, eMonth::january, 1) ? 0 : state->remainingMonths(currentDate, state->mComplyStartDate);
    const int remainingMonths = std::max(0, comply - elapsedMonths);
    return std::to_string(remainingMonths);
}

std::string eFulfillRequestEvent::requestInfo(int stock, const eDate &currentDate) const
{
    const auto resName = eResourceTypeHelpers::typeLongName(mResource);
    const int requested = mCount;
    const bool overdue = isOverdue(currentDate);
    const int daysDiff = overdue ? currentDate - complyDate() : complyDate() - currentDate;
    const int remainingMonths = daysDiff / 31;
    std::string status;
    if (!overdue)
    {
        status = eLanguage::zeusText(212, 63); // [months_remaining] months remain
        eStringHelpers::replaceAll(status, "[months_remaining]",
                                   std::to_string(remainingMonths));
    }
    else
    {
        status = std::to_string(remainingMonths) + " " +
                 eLanguage::zeusText(5, 150) + " " +
                 eLanguage::zeusText(5, 205);
    }
    auto stockText = eLanguage::zeusText(44, 278); // in stock
    auto result = std::to_string(requested) + " " + resName +
                  " (" + std::to_string(stock) + " " + stockText + ")";
    if(!status.empty()) result += ", " + status;
    return result;
}

std::string eFulfillRequestEvent::dispatchText(int stock, const eDate &currentDate) const
{
    return eLanguage::zeusText(5, 12) + " " +
           requestInfo(stock, currentDate) + "?";
}
