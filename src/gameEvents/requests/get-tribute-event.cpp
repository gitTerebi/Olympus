#include "get-tribute-event.h"

#include "engine/tribute.h"
#include "engine/game-board.h"
#include "engine/eevent.h"
#include "engine/eeventdata.h"
#include "elanguage.h"
#include "emessages.h"
#include "estringhelpers.h"
#include "fileIO/esavearchive.h"

#include <algorithm>
#include <vector>

namespace
{
    struct ePayTributeState
    {
        eEvent fEvent;
        int fPopupMonth;
        int fComplyMonths;
        eEvent fNextEvent;
    };

    const std::vector<ePayTributeState> &payTributeStates()
    {
        static const std::vector<ePayTributeState> states = {
            {eEvent::generalRequestTributeInitial, 0, 2,
             eEvent::generalRequestTributeReminder},
            {eEvent::generalRequestTributeReminder, 2, 6,
             eEvent::generalRequestTributeOverdue},
            {eEvent::generalRequestTributeOverdue, 8, 12,
             eEvent::generalRequestTributeWarning},
            {eEvent::generalRequestTributeWarning, 14, 6,
             eEvent::generalRequestTributeTooLate},
            {eEvent::generalRequestTributeTooLate, 20, 0,
             eEvent::generalRequestTributeTooLate},
        };
        return states;
    }

    const ePayTributeState &payTributeState(const eEvent event)
    {
        const auto &states = payTributeStates();
        for (const auto &state : states)
        {
            if (state.fEvent == event)
                return state;
        }
        return states.front();
    }

    int payTributeStateIndex(const eEvent event)
    {
        const auto &states = payTributeStates();
        for (int i = 0; i < static_cast<int>(states.size()); ++i)
        {
            if (states[i].fEvent == event)
                return i;
        }
        return -1;
    }

    bool payTributeTerminalState(const eEvent event)
    {
        return event == eEvent::generalRequestTributeTooLate;
    }
}

GetTributeEvent::GetTributeEvent(
    const eCityId cid,
    const eGameEventBranch branch,
    GameBoard &board) : eGameEvent(cid, eGameEventType::getTribute, branch, board) {}

GetTributeEvent::~GetTributeEvent()
{
    const auto board = gameBoard();
    if (board && isMainEvent())
    {
        board->removeTributeRequest(this);
    }
}

void GetTributeEvent::initialize(const stdsptr<WorldCity> &c)
{
    mCity = c;
    if (!mCity)
        return;
    const auto board = gameBoard();
    if (!board)
        return;
    const auto diff = board->difficulty(board->personPlayer());
    const auto tribute = TributeHelpers::payTribute(*mCity, diff);
    mResource = tribute.fType;
    mCount = tribute.fCount;
    activate();
}

void GetTributeEvent::trigger()
{
    if (!mCity)
        return;
    const auto board = gameBoard();
    if (!board)
        return;
    if (!mActive && isMainEvent())
        return;

    eEventData ed(board->personPlayer());
    ed.fType = eMessageEventType::generalRequestGranted;
    ed.fCity = mCity;
    ed.fResourceType = mResource;
    ed.fResourceCount = mCount;
    ed.fTime = popupComplyMonths();
    ed.fEventRuntimeId = runtimeId();

    const bool canDispatch = !payTributeTerminalState(mEvent);
    if (!canDispatch)
    {
        if (isMainEvent())
            finish(GetTributeResult::refuse);
        return;
    }

    if (mResource == eResourceType::drachmas)
    {
        const auto cids = board->personPlayerCitiesOnBoard();
        if (!cids.empty() && board->drachmas(board->personPlayer()) >= mCount)
        {
            const auto cid = cids[0];
            ed.fPrimaryResponse = static_cast<int>(eResponse::dispatch);
        }
    }
    else if (canDispatch)
    {
        const auto cids = board->personPlayerCitiesOnBoard();
        for (const auto cid : cids)
        {
            const int avCount = board->resourceCount(cid, mResource);
            ed.fCityNames[cid] = board->cityName(cid);
            ed.fCSpaceCount[cid] = avCount;
            if (avCount >= mCount)
            {
                ed.fCityConditionalResponses[cid] = static_cast<int>(eResponse::dispatch);
            }
        }
    }

    if (!payTributeTerminalState(mEvent))
    {
        ed.fSecondaryResponse = static_cast<int>(eResponse::postpone);
    }

    ed.fTertiaryResponse = static_cast<int>(eResponse::refuse);

    board->event(stepEvent(), ed);
}

void GetTributeEvent::respond(const int response, const eCityId city)
{
    switch (static_cast<eResponse>(response))
    {
    case eResponse::dispatch:
        if (city == eCityId::neutralAggresive)
        {
            const auto board = gameBoard();
            const auto cids = board ? board->personPlayerCitiesOnBoard() : std::vector<eCityId>();
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
        if (const auto request = mainEvent<GetTributeEvent>())
            request->finish(GetTributeResult::refuse);
        break;
    }
}

std::string GetTributeEvent::longName() const
{
    auto tmpl = eLanguage::text("pay_tribute");
    const auto none = eLanguage::text("none");
    const auto ctstr = mCity ? mCity->name() : none;
    eStringHelpers::replace(tmpl, "%1", ctstr);
    return tmpl;
}

bool GetTributeEvent::finished() const
{
    return eGameEvent::finished() && !mActive;
}

eCityRequest GetTributeEvent::cityRequest() const
{
    eCityRequest request;
    request.fCity = mCity;
    request.fType = mResource;
    request.fCount = mCount;
    return request;
}

void GetTributeEvent::dispatch(const eCityId cid)
{
    const auto board = gameBoard();
    if (!board)
        return;
    const auto request = mainEvent<GetTributeEvent>();
    const auto state = request ? request->mComplyEvent : mEvent;
    if (payTributeTerminalState(state))
        return;

    board->takeResource(cid, mResource, mCount);
    if (request)
    {
        const int lateStep = payTributeStateIndex(
            eEvent::generalRequestTributeWarning);
        const int currentStep = payTributeStateIndex(
            request->mComplyEvent);
        const bool overdue = lateStep >= 0 &&
                             currentStep >= lateStep;
        request->finish(overdue ? GetTributeResult::tooLate : GetTributeResult::comply);
    }
}

void GetTributeEvent::postpone()
{
    const auto request = mainEvent<GetTributeEvent>();
    if (!request)
    {
        return;
    }

    request->mPostponed = true;
}

std::string GetTributeEvent::dispatchText(
    const int stock, const eDate &currentDate) const
{
    const auto resName = eResourceTypeHelpers::typeLongName(mResource);
    (void)currentDate;
    const int months = complyMonths();
    auto status = eLanguage::zeusText(212, 63); // [months_remaining] months remain
    eStringHelpers::replaceAll(status, "[months_remaining]",
                               std::to_string(months));
    auto stockText = eLanguage::zeusText(44, 278); // in stock
    return eLanguage::zeusText(5, 12) + " " +
           std::to_string(mCount) + " " + resName +
           " (" + std::to_string(stock) + " " + stockText + "), " +
           status + "?";
}

std::string GetTributeEvent::overdueStatusText(
    const eDate &currentDate) const
{
    const auto request = const_cast<GetTributeEvent *>(this)
                             ->mainEvent<GetTributeEvent>();
    const auto state = request ? request : this;
    // Sidebar counts down from the current step's comply window.
    const int comply = state->complyMonths();
    const int elapsedMonths =
        state->mComplyStartDate == eDate(1, eMonth::january, 1) ?
            0 :
            state->remainingMonths(currentDate, state->mComplyStartDate);
    const int remainingMonths = std::max(0, comply - elapsedMonths);
    return std::to_string(remainingMonths);
}

void GetTributeEvent::serializeFields(eSaveArchive &ar)
{
    eGameEvent::serializeFields(ar);
    ar.worldCityField("city", worldBoard(), mCity);
    ar.field("resource", mResource, eResourceType::drachmas);
    ar.field("count", mCount, 0);
    ar.field("active", mActive, false);
    ar.field("postponed", mPostponed, false);
    ar.field("event", mEvent, eEvent::generalRequestTributeInitial);
    ar.field("complyEvent", mComplyEvent, eEvent::generalRequestTributeInitial);
    ar.field("requestStep", mRequestStep, 0);
    ar.dateField("requestDate", mRequestDate);
    ar.dateField("requestDeadline", mRequestDeadline);
    ar.dateField("complyStartDate", mComplyStartDate);
    if (ar.reading() && mPostponed && mRequestStep == 0)
    {
        mRequestStep = 2;
    }
}

void GetTributeEvent::activate()
{
    if (mActive)
        return;
    const auto board = gameBoard();
    if (!board)
        return;
    mActive = true;
    mPostponed = false;
    mRequestStep = 0;
    mEvent = payTributeStates().front().fEvent;
    mComplyEvent = mEvent;
    mRequestDate = board->date();
    mRequestDeadline = board->date();
    mComplyStartDate = board->date();
    mRequestDeadline.nextMonths(complyMonths());
    board->addTributeRequest(this);
}

bool GetTributeEvent::isPostponed() const
{
    const int overdueStep = payTributeStateIndex(
        eEvent::generalRequestTributeOverdue);
    const int currentStep = payTributeStateIndex(mComplyEvent);
    return overdueStep >= 0 && currentStep >= overdueStep;
}

void GetTributeEvent::advanceIfNeeded(const eDate &currentDate)
{
    const auto board = gameBoard();
    if (!board || !isMainEvent() || finished())
        return;
    if (mRequestDate == eDate(1, eMonth::january, 1))
        return;
    const auto nextEvent = payTributeState(mComplyEvent).fNextEvent;
    auto nextDate = mRequestDate;
    nextDate.nextMonths(payTributeState(nextEvent).fPopupMonth);
    if (currentDate < nextDate)
        return;
    advanceToNextStep(*board);
}

void GetTributeEvent::advanceToNextStep(GameBoard &board)
{
    const auto nextEvent = payTributeState(mComplyEvent).fNextEvent;
    if (payTributeTerminalState(nextEvent))
    {
        finish(GetTributeResult::refuse);
        return;
    }

    clearConsequences();
    mPostponed = false;
    mRequestStep = payTributeStateIndex(nextEvent);
    mEvent = nextEvent;
    mComplyEvent = nextEvent;
    mComplyStartDate = board.date();
    mRequestDeadline = board.date();
    mRequestDeadline.nextMonths(complyMonths());

    eEventData ed(board.personPlayer());
    ed.fType = eMessageEventType::generalRequestGranted;
    ed.fCity = mCity;
    ed.fResourceType = mResource;
    ed.fResourceCount = mCount;
    ed.fTime = popupComplyMonths();
    ed.fEventRuntimeId = runtimeId();

    if (mResource == eResourceType::drachmas)
    {
        const auto cids = board.personPlayerCitiesOnBoard();
        if (!cids.empty() && board.drachmas(board.personPlayer()) >= mCount)
        {
            ed.fPrimaryResponse = static_cast<int>(eResponse::dispatch);
        }
    }
    else
    {
        const auto cids = board.personPlayerCitiesOnBoard();
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

    ed.fSecondaryResponse = static_cast<int>(eResponse::postpone);
    ed.fTertiaryResponse = static_cast<int>(eResponse::refuse);
    board.event(stepEvent(), ed);
}

eEvent GetTributeEvent::stepEvent() const
{
    return mEvent;
}

int GetTributeEvent::complyMonths() const
{
    return payTributeState(mComplyEvent).fComplyMonths;
}

int GetTributeEvent::popupComplyMonths() const
{
    return payTributeState(mEvent).fComplyMonths;
}

void GetTributeEvent::finish(const GetTributeResult result)
{
    const auto board = gameBoard();
    if (!board)
        return;
    clearConsequences();
    board->removeTributeRequest(this);
    mActive = false;
    setRepeat(0);

    eEventData ed(board->personPlayer());
    ed.fType = eMessageEventType::resourceGranted;
    ed.fCity = mCity;
    ed.fResourceType = mResource;
    ed.fResourceCount = mCount;

    if (result == GetTributeResult::refuse)
    {
        board->event(eEvent::generalRequestTributeRefuse, ed);
        if (mCity)
            board->changeCityAttitude(mCity, -10, board->personPlayer());
    }
    else if (result == GetTributeResult::tooLate)
    {
        board->event(eEvent::generalRequestTributeTooLate, ed);
        if (mCity)
            board->changeCityAttitude(mCity, 5, board->personPlayer());
    }
    else
    {
        board->event(eEvent::generalRequestTributeComply, ed);
        if (mCity)
            board->changeCityAttitude(mCity, 10, board->personPlayer());
    }
}

int GetTributeEvent::remainingMonths(
    const eDate &deadline, const eDate &current) const
{
    const int daysDiff = deadline - current;
    return (daysDiff + 30) / 31;
}
