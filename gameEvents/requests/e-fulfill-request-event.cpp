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

    int displayMonthsForRequestStep(const eFulfillRequestEvent *request, const int requestStep)
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

    const auto request = mainEvent<eFulfillRequestEvent>();
    if (request)
    {
        const int popupStep = mRequestStep;
        const int complyStep = request->mComplyStep;
        if (isMainEvent() ||
            request->mComplyStartDate == eDate(1, eMonth::january, 1) ||
            (request->mPostponed && popupStep > complyStep))
        {
            request->mRequestStep = mRequestStep;
            request->mComplyStep = mRequestStep;
            request->mComplyStartDate = board->date();
            request->mRequestDeadline = board->date();
            request->mRequestDeadline.nextMonths(complyMonths());
            request->mPostponed = false;
        }
    }

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
    addConsequence(request);
    request->initializeDate(board.date());
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
    ed.fTime = displayMonthsForRequestStep(this, mRequestStep);
    ed.fGod = mGod;
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
        auto dispatchAction = [this, cid]()
        {
            dispatch(cid);
        };
        ed.fPrimaryAction = dispatchAction;
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
            auto dispatchNowAction = [this, cid]()
            {
                dispatch(cid);
            };
            ed.fCityConditionalActions[cid] = dispatchNowAction;
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
        auto postponeAction = [this]()
        {
            postpone();
        };
        ed.fSecondaryAction = postponeAction;
    }
}

void eFulfillRequestEvent::addRefuseButton(eGameBoard &board, eEventData &ed)
{
    const auto request = mainEvent<eFulfillRequestEvent>();
    auto refuseAction = [request, &board]()
    {
        if (!request)
            return;
        request->finish(eReceiveRequestResult::refuse);
    };
    ed.fTertiaryAction = refuseAction;
}

void eFulfillRequestEvent::postpone()
{
    const auto board = gameBoard();
    if (!board)
        return;
    const auto request = mainEvent<eFulfillRequestEvent>();
    if (!request)
        return;

    request->mPostponed = true;
    const int currentStep = request->mRequestStep;
    request->clearConsequences();

    const auto &states = fulfillRequestStates();
    const int nextStep = currentStep + 1;
    if (nextStep < static_cast<int>(states.size()))
    {
        auto popupDate = request->mRequestDate;
        popupDate.nextMonths(states[nextStep].fPopupMonth);
        request->scheduleStep(nextStep, popupDate);
    }
}

void eFulfillRequestEvent::scheduleStep(const int step, const eDate &date)
{
    const auto board = gameBoard();
    if (!board)
        return;
    const auto &states = fulfillRequestStates();
    if (step < 0 || step >= static_cast<int>(states.size()))
        return;
    const auto e = e::make_shared<eFulfillRequestEvent>(
        cityId(), eGameEventBranch::child, *board);
    e->copyFrom(*this, step);
    e->initializeDate(date);
    addConsequence(e);
}

void eFulfillRequestEvent::copyFrom(
    const eFulfillRequestEvent &src, const int step)
{
    mCity = src.mCity;
    mResource = src.mResource;
    mCount = src.mCount;
    mGod = src.mGod;
    mRequestType = src.mRequestType;
    mRequestResult = src.mRequestResult;
    mRequestFinished = src.mRequestFinished;
    mRequestStep = step;
    mRequestId = src.mRequestId;
    mComplyStep = src.mComplyStep;
    mPostponed = src.mPostponed;
    mRequestDate = src.mRequestDate;
    mRequestDeadline = src.mRequestDeadline;
    mComplyStartDate = src.mComplyStartDate;
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

void eFulfillRequestEvent::write(eWriteStream &dst) const
{
    eGameEvent::write(dst);
    eSaveArchive ar(dst);
    const_cast<eFulfillRequestEvent *>(this)->serialize(ar);
}

void eFulfillRequestEvent::read(eReadStream &src)
{
    eGameEvent::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eFulfillRequestEvent::serialize(eSaveArchive &ar)
{
    if (ar.reading())
    {
        eResourceEventValue::read(ar.readStream());
        eCountEventValue::read(ar.readStream());
        eCityEventValue::read(ar.readStream(), *gameBoard());
        eGodEventValue::read(ar.readStream());
    }
    else
    {
        eResourceEventValue::write(ar.writeStream());
        eCountEventValue::write(ar.writeStream());
        eCityEventValue::write(ar.writeStream());
        eGodEventValue::write(ar.writeStream());
    }
    ar.field("mRequestType", mRequestType);
    ar.field("mRequestResult", mRequestResult);
    ar.field("mRequestFinished", mRequestFinished);
    ar.field("mRequestStep", mRequestStep);
    ar.field("mRequestId", mRequestId);
    ar.field("mComplyStep", mComplyStep);
    ar.field("mPostponed", mPostponed);
    int requestDay = mRequestDate.day();
    auto requestMonth = mRequestDate.month();
    int requestYear = mRequestDate.year();
    ar.field("mRequestDate.day", requestDay);
    ar.field("mRequestDate.month", requestMonth);
    ar.field("mRequestDate.year", requestYear);
    if (ar.reading())
    {
        mRequestDate = eDate(requestDay, requestMonth, requestYear);
    }
    int deadlineDay = mRequestDeadline.day();
    auto deadlineMonth = mRequestDeadline.month();
    int deadlineYear = mRequestDeadline.year();
    ar.field("mRequestDeadline.day", deadlineDay);
    ar.field("mRequestDeadline.month", deadlineMonth);
    ar.field("mRequestDeadline.year", deadlineYear);
    if (ar.reading())
    {
        mRequestDeadline = eDate(deadlineDay, deadlineMonth, deadlineYear);
        if (mPostponed && mRequestStep == 0)
        {
            mRequestStep = 2;
        }
    }
    int complyStartDay = mComplyStartDate.day();
    auto complyStartMonth = mComplyStartDate.month();
    int complyStartYear = mComplyStartDate.year();
    ar.field("complyStartDate.day", complyStartDay);
    ar.field("complyStartDate.month", complyStartMonth);
    ar.field("complyStartDate.year", complyStartYear);
    if (ar.reading())
    {
        mComplyStartDate = eDate(
            complyStartDay, complyStartMonth, complyStartYear);
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
        mCity->incAttitude(-10, playerId());
    else if (mCity && result == eReceiveRequestResult::tooLate)
        mCity->incAttitude(5, playerId());
    else if (mCity)
        mCity->incAttitude(10, playerId());

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
    return std::to_string(requested) + " " + resName +
           " (" + std::to_string(stock) + " " + stockText + "), " + status;
}

std::string eFulfillRequestEvent::dispatchText(int stock, const eDate &currentDate) const
{
    return eLanguage::zeusText(5, 12) + " " +
           requestInfo(stock, currentDate) + "?";
}
