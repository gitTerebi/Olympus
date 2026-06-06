#include "receive-tribute-event.h"

#include "request-state.h"

#include "engine/tribute.h"
#include "engine/game-board.h"
#include "engine/eevent.h"
#include "engine/eeventdata.h"
#include "elanguage.h"
#include "emessages.h"
#include "estringhelpers.h"
#include "fileIO/esavearchive.h"

#include <vector>

namespace
{
    struct ePayTributeState
    {
        eEvent fEvent;
        eEvent fNextEvent;
        RequestState fState;
    };

    const std::vector<ePayTributeState> &payTributeStates()
    {
        static const std::vector<ePayTributeState> states = {
            {eEvent::generalRequestTributeInitial,
             eEvent::generalRequestTributeReminder, {0, 2}},
            {eEvent::generalRequestTributeReminder,
             eEvent::generalRequestTributeOverdue, {2, 6}},
            {eEvent::generalRequestTributeOverdue,
             eEvent::generalRequestTributeWarning, {8, 12}},
            {eEvent::generalRequestTributeWarning,
             eEvent::generalRequestTributeTooLate, {14, 6}},
            {eEvent::generalRequestTributeTooLate,
             eEvent::generalRequestTributeTooLate, {20, 0}},
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

ReceiveTributeEvent::ReceiveTributeEvent(
    const eCityId cid,
    const eGameEventBranch branch,
    GameBoard &board) : eGameEvent(cid, eGameEventType::receiveTribute, branch, board) {}

ReceiveTributeEvent::~ReceiveTributeEvent()
{
    const auto board = gameBoard();
    if (board && isMainEvent())
    {
        board->removeTributeRequest(this);
    }
}

void ReceiveTributeEvent::initialize(const stdsptr<WorldCity> &c)
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

void ReceiveTributeEvent::trigger()
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

    if (payTributeTerminalState(mEvent))
    {
        if (isMainEvent())
            finish(GetTributeResult::refuse);
        return;
    }

    fillEventDataActions(ed);

    board->event(stepEvent(), ed);
}

void ReceiveTributeEvent::fillEventDataActions(eEventData &ed)
{
    const auto board = gameBoard();
    const auto request = mainEvent<ReceiveTributeEvent>();
    if (!board || !request || !request->mActive)
        return;
    if (payTributeTerminalState(request->mEvent))
        return;

    ed.fType = eMessageEventType::generalRequestGranted;
    ed.fEventRuntimeId = request->runtimeId();
    ed.fCity = request->mCity;
    ed.fResourceType = request->mResource;
    ed.fResourceCount = request->mCount;
    ed.fTime = request->popupComplyMonths();
    ed.fCloseResponse = -1;
    ed.fPrimaryResponse = -1;
    ed.fCityNames.clear();
    ed.fCityConditionalResponses.clear();
    ed.fCSpaceCount.clear();
    ed.fSecondaryResponse = -1;
    ed.fTertiaryResponse = -1;

    addRequestDispatchResponses(*board, ed, board->personPlayer(),
                                request->mResource, request->mCount,
                                static_cast<int>(eResponse::dispatch));
    ed.fSecondaryResponse = static_cast<int>(eResponse::postpone);
    ed.fTertiaryResponse = static_cast<int>(eResponse::refuse);
}

void ReceiveTributeEvent::respond(const int response, const eCityId city)
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
        if (const auto request = mainEvent<ReceiveTributeEvent>())
            request->finish(GetTributeResult::refuse);
        break;
    }
}

std::string ReceiveTributeEvent::longName() const
{
    auto tmpl = eLanguage::text("pay_tribute");
    const auto none = eLanguage::text("none");
    const auto ctstr = mCity ? mCity->name() : none;
    eStringHelpers::replace(tmpl, "%1", ctstr);
    return tmpl;
}

bool ReceiveTributeEvent::finished() const
{
    return eGameEvent::finished() && !mActive;
}

eCityRequest ReceiveTributeEvent::cityRequest() const
{
    eCityRequest request;
    request.fCity = mCity;
    request.fType = mResource;
    request.fCount = mCount;
    return request;
}

void ReceiveTributeEvent::dispatch(const eCityId cid)
{
    const auto board = gameBoard();
    if (!board)
        return;
    const auto request = mainEvent<ReceiveTributeEvent>();
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

void ReceiveTributeEvent::postpone()
{
    const auto request = mainEvent<ReceiveTributeEvent>();
    if (!request)
    {
        return;
    }

    request->mPostponed = true;
}

std::string ReceiveTributeEvent::dispatchText(
    const int stock, const eDate &currentDate) const
{
    (void)currentDate;
    const auto info = resourceRequestInfo(mResource, mCount, stock, false,
                                          complyMonths());
    return resourceDispatchText(info);
}

std::string ReceiveTributeEvent::overdueStatusText(
    const eDate &currentDate) const
{
    const auto request = const_cast<ReceiveTributeEvent *>(this)
                             ->mainEvent<ReceiveTributeEvent>();
    const auto state = request ? request : this;
    return requestCountdownText(state->complyMonths(),
                                state->mComplyStartDate,
                                currentDate);
}

void ReceiveTributeEvent::serializeFields(eSaveArchive &ar)
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

void ReceiveTributeEvent::activate()
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

bool ReceiveTributeEvent::isPostponed() const
{
    const int overdueStep = payTributeStateIndex(
        eEvent::generalRequestTributeOverdue);
    const int currentStep = payTributeStateIndex(mComplyEvent);
    return overdueStep >= 0 && currentStep >= overdueStep;
}

void ReceiveTributeEvent::advanceIfNeeded(const eDate &currentDate)
{
    const auto board = gameBoard();
    if (!board || !isMainEvent() || finished())
        return;
    if (mRequestDate == eDate(1, eMonth::january, 1))
        return;
    const auto nextEvent = payTributeState(mComplyEvent).fNextEvent;
    auto nextDate = mRequestDate;
    nextDate.nextMonths(payTributeState(nextEvent).fState.fPopupMonth);
    if (currentDate < nextDate)
        return;
    advanceToNextStep(*board);
}

void ReceiveTributeEvent::advanceToNextStep(GameBoard &board)
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

    fillEventDataActions(ed);
    board.event(stepEvent(), ed);
}

eEvent ReceiveTributeEvent::stepEvent() const
{
    return mEvent;
}

int ReceiveTributeEvent::complyMonths() const
{
    return payTributeState(mComplyEvent).fState.fComplyMonths;
}

int ReceiveTributeEvent::popupComplyMonths() const
{
    return payTributeState(mEvent).fState.fComplyMonths;
}

void ReceiveTributeEvent::finish(const GetTributeResult result)
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
