#include "e-pay-tribute-event.h"

#include "engine/e-tribute.h"
#include "engine/e-game-board.h"
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

ePayTributeEvent::ePayTributeEvent(
    const eCityId cid,
    const eGameEventBranch branch,
    eGameBoard &board) : eGameEvent(cid, eGameEventType::payTribute, branch, board) {}

ePayTributeEvent::~ePayTributeEvent()
{
    const auto board = gameBoard();
    if (board && isMainEvent())
    {
        board->removeTributeRequest(this);
    }
}

void ePayTributeEvent::initialize(const stdsptr<eWorldCity> &c)
{
    mCity = c;
    if (!mCity)
        return;
    const auto board = gameBoard();
    if (!board)
        return;
    const auto diff = board->difficulty(board->personPlayer());
    const auto tribute = eTributeHelpers::payTribute(*mCity, diff);
    mResource = tribute.fType;
    mCount = tribute.fCount;
    activate();
}

void ePayTributeEvent::trigger()
{
    if (!mCity)
        return;
    const auto board = gameBoard();
    if (!board)
        return;
    if (!mActive && isMainEvent())
        return;

    const auto request = mainEvent<ePayTributeEvent>();
    if (request)
    {
        const int popupStep = payTributeStateIndex(mEvent);
        const int complyStep = payTributeStateIndex(
            request->mComplyEvent);
        if (isMainEvent() ||
            request->mComplyStartDate == eDate(1, eMonth::january, 1) ||
            (request->mPostponed && popupStep > complyStep))
        {
            request->mRequestStep = mRequestStep;
            request->mEvent = mEvent;
            request->mComplyEvent = mEvent;
            request->mComplyStartDate = board->date();
            request->mRequestDeadline = board->date();
            request->mRequestDeadline.nextMonths(complyMonths());
            request->mPostponed = false;
        }
    }

    eEventData ed(board->personPlayer());
    ed.fType = eMessageEventType::generalRequestGranted;
    ed.fCity = mCity;
    ed.fResourceType = mResource;
    ed.fResourceCount = mCount;
    ed.fTime = popupComplyMonths();

    const bool canDispatch = !payTributeTerminalState(mEvent);
    if (canDispatch && mResource == eResourceType::drachmas)
    {
        const auto cids = board->personPlayerCitiesOnBoard();
        if (!cids.empty() && board->drachmas(board->personPlayer()) >= mCount)
        {
            const auto cid = cids[0];
            ed.fPrimaryAction = [this, cid]()
            {
                dispatch(cid);
            };
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
                ed.fCityConditionalActions[cid] = [this, cid]()
                {
                    dispatch(cid);
                };
            }
        }
    }

    if (!payTributeTerminalState(mEvent))
    {
        ed.fSecondaryAction = [this]()
        {
            postpone();
        };
    }

    ed.fTertiaryAction = [this]()
    {
        const auto request = mainEvent<ePayTributeEvent>();
        if (request)
            request->finish(ePayTributeResult::refuse);
    };

    board->event(stepEvent(), ed);
}

std::string ePayTributeEvent::longName() const
{
    auto tmpl = eLanguage::text("pay_tribute");
    const auto none = eLanguage::text("none");
    const auto ctstr = mCity ? mCity->name() : none;
    eStringHelpers::replace(tmpl, "%1", ctstr);
    return tmpl;
}

bool ePayTributeEvent::finished() const
{
    return eGameEvent::finished() && !mActive;
}

eCityRequest ePayTributeEvent::cityRequest() const
{
    eCityRequest request;
    request.fCity = mCity;
    request.fType = mResource;
    request.fCount = mCount;
    return request;
}

void ePayTributeEvent::dispatch(const eCityId cid)
{
    const auto board = gameBoard();
    if (!board)
        return;
    const auto request = mainEvent<ePayTributeEvent>();
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
        request->finish(overdue ? ePayTributeResult::tooLate : ePayTributeResult::comply);
    }
}

void ePayTributeEvent::postpone()
{
    const auto board = gameBoard();
    if (!board)
        return;
    const auto request = mainEvent<ePayTributeEvent>();
    if (!request)
    {
        return;
    }

    request->mPostponed = true;
    const int currentStep = payTributeStateIndex(
        request->mComplyEvent);
    request->clearConsequences();

    if (currentStep < 0)
    {
        return;
    }
    const auto nextEvent = payTributeState(request->mComplyEvent).fNextEvent;
    const auto &nextState = payTributeState(nextEvent);
    auto popupDate = request->mRequestDate;
    popupDate.nextMonths(nextState.fPopupMonth);
    request->scheduleStep(payTributeStateIndex(nextEvent), popupDate);
}

std::string ePayTributeEvent::dispatchText(
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

std::string ePayTributeEvent::overdueStatusText(
    const eDate &currentDate) const
{
    const auto request = const_cast<ePayTributeEvent *>(this)
                             ->mainEvent<ePayTributeEvent>();
    const auto state = request ? request : this;
    // Sidebar counts down from the current step's comply window.
    const int comply = state->complyMonths();
    const int elapsedMonths =
        state->mComplyStartDate == eDate(1, eMonth::january, 1) ?
            0 :
            state->remainingMonths(currentDate, state->mComplyStartDate);
    const int remainingMonths = std::max(0, comply - elapsedMonths);
    const int currentYear = currentDate.year();
    const int currentMonth = static_cast<int>(currentDate.month());
    if (state->mDebugPrintYear != currentYear ||
        state->mDebugPrintMonth != currentMonth)
    {
        state->mDebugPrintYear = currentYear;
        state->mDebugPrintMonth = currentMonth;
    }
    return std::to_string(remainingMonths);
}

void ePayTributeEvent::write(eWriteStream &dst) const
{
    eGameEvent::write(dst);
    eSaveArchive ar(dst);
    const_cast<ePayTributeEvent *>(this)->serialize(ar);
}

void ePayTributeEvent::read(eReadStream &src)
{
    eGameEvent::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void ePayTributeEvent::serialize(eSaveArchive &ar)
{
    if (ar.reading())
    {
        ar.readStream().readCity(worldBoard(), [this](const stdsptr<eWorldCity> &c)
                                 { mCity = c; });
    }
    else
    {
        ar.writeStream().writeCity(mCity.get());
    }
    ar.field("resource", mResource);
    ar.field("count", mCount);
    ar.field("active", mActive);
    ar.field("postponed", mPostponed);
    ar.field("event", mEvent);
    ar.field("complyEvent", mComplyEvent);
    ar.field("requestStep", mRequestStep);
    int requestDay = mRequestDate.day();
    auto requestMonth = mRequestDate.month();
    int requestYear = mRequestDate.year();
    ar.field("requestDate.day", requestDay);
    ar.field("requestDate.month", requestMonth);
    ar.field("requestDate.year", requestYear);
    if (ar.reading())
    {
        mRequestDate = eDate(requestDay, requestMonth, requestYear);
    }
    int deadlineDay = mRequestDeadline.day();
    auto deadlineMonth = mRequestDeadline.month();
    int deadlineYear = mRequestDeadline.year();
    ar.field("requestDeadline.day", deadlineDay);
    ar.field("requestDeadline.month", deadlineMonth);
    ar.field("requestDeadline.year", deadlineYear);
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

void ePayTributeEvent::activate()
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
    board->addTributeRequest(this);
}

void ePayTributeEvent::scheduleStep(const int step, const eDate &date)
{
    const auto board = gameBoard();
    if (!board)
        return;
    const auto &states = payTributeStates();
    if (step < 0 || step >= static_cast<int>(states.size()))
    {
        return;
    }
    const auto e = e::make_shared<ePayTributeEvent>(
        cityId(), eGameEventBranch::child, *board);
    e->copyFrom(*this, step, states[step].fEvent);
    e->initializeDate(date);
    addConsequence(e);
}

void ePayTributeEvent::copyFrom(
    const ePayTributeEvent &src, const int step, const eEvent event)
{
    mCity = src.mCity;
    mResource = src.mResource;
    mCount = src.mCount;
    mActive = isMainEvent() && src.mActive;
    mPostponed = src.mPostponed;
    mRequestStep = step;
    mEvent = event;
    mComplyEvent = src.mComplyEvent;
    mRequestDate = src.mRequestDate;
    mRequestDeadline = src.mRequestDeadline;
    mComplyStartDate = src.mComplyStartDate;
}

bool ePayTributeEvent::isPostponed() const
{
    const int overdueStep = payTributeStateIndex(
        eEvent::generalRequestTributeOverdue);
    const int currentStep = payTributeStateIndex(mComplyEvent);
    return overdueStep >= 0 && currentStep >= overdueStep;
}

eEvent ePayTributeEvent::stepEvent() const
{
    return mEvent;
}

int ePayTributeEvent::complyMonths() const
{
    return payTributeState(mComplyEvent).fComplyMonths;
}

int ePayTributeEvent::popupComplyMonths() const
{
    return payTributeState(mEvent).fComplyMonths;
}

void ePayTributeEvent::finish(const ePayTributeResult result)
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

    if (result == ePayTributeResult::refuse)
    {
        board->event(eEvent::generalRequestTributeRefuse, ed);
        if (mCity)
            mCity->incAttitude(-10, board->personPlayer());
    }
    else if (result == ePayTributeResult::tooLate)
    {
        board->event(eEvent::generalRequestTributeTooLate, ed);
        if (mCity)
            mCity->incAttitude(5, board->personPlayer());
    }
    else
    {
        board->event(eEvent::generalRequestTributeComply, ed);
        if (mCity)
            mCity->incAttitude(10, board->personPlayer());
    }
}

int ePayTributeEvent::remainingMonths(
    const eDate &deadline, const eDate &current) const
{
    const int daysDiff = deadline - current;
    return (daysDiff + 30) / 31;
}
