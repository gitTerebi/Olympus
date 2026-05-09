#include "e-fulfill-request-event.h"
#include "e-receive-request-type.h"
#include "e-receive-request-postpone.h"

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
    deadline.nextMonths(warningMonths());
    mRequestDeadline = deadline;
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
    const auto pid = playerId();
    ed.fType = eMessageEventType::resourceGranted;
    const auto *msgs = receiveRequestMessages(mRequestType, *mCity);
    if (mRequestResult == eReceiveRequestResult::refuse)
    {
        const auto event = receiveRequestFinishEvent(
            mRequestType, *mCity, eReceiveRequestFinish::refuse);
        board.event(event, ed);
        mCity->incAttitude(-15, pid);
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
        mCity->incAttitude(-5, pid);
        const auto &reason = msgs->fTooLateReason;
        const auto me = mainEvent<eFulfillRequestEvent>();
        me->finished(*me->mTooLateTrigger, reason);
        return;
    }

    const auto event = receiveRequestFinishEvent(
        mRequestType, *mCity, eReceiveRequestFinish::comply);
    board.event(event, ed);
    mCity->incAttitude(10, pid);
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
    if (canPostponeRequestStep(this))
    {
        // Reminder stages advance here too, but only final postpone extends.
        auto postponeAction = [request, &board]()
        {
            advanceRequestStep(request, board);
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
        request->refuseRequest(board);
    };
    ed.fTertiaryAction = refuseAction;
}

void eFulfillRequestEvent::refuseRequest(eGameBoard &board)
{
    board.removeCityRequest(this);
    mRequestFinished = true;
    mRequestResult = eReceiveRequestResult::refuse;

    const auto pid = mRequestType == eReceiveRequestType::tribute ? board.personPlayer() : playerId();
    eEventData ed(pid);
    ed.fCity = mCity;
    ed.fResourceType = mResource;
    ed.fResourceCount = mCount;
    ed.fGod = mGod;
    showRequestFinished(board, ed);
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
    if (ar.reading() && mRequestId >= sNextReceiveRequestId)
    {
        sNextReceiveRequestId = mRequestId + 1;
    }
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
    fulfillWithoutCost();
}

void eFulfillRequestEvent::fulfillWithoutCost()
{
    const auto board = gameBoard();
    if (!board)
        return;
    clearConsequences();
    board->removeCityRequest(mainEvent<eFulfillRequestEvent>());
    mainEvent<eFulfillRequestEvent>()->mRequestFinished = true;
    const auto cid = cityId();
    const auto e = e::make_shared<eFulfillRequestEvent>(
        cid, eGameEventBranch::child, *board);
    const auto currentDate = board->date();
    e->set(*this, mRequestStep, true);
    e->mRequestResult = mRequestStep > 1 ? eReceiveRequestResult::tooLate : eReceiveRequestResult::comply;
    const auto edate = currentDate + 3 * 31;
    e->initializeDate(edate);
    addConsequence(e);
}

void eFulfillRequestEvent::initialize(
    const int requestStep,
    const eResourceType res,
    const int count,
    const stdsptr<eWorldCity> &c,
    const int warningMonths,
    const bool showResultMessage)
{
    mRequestStep = requestStep;
    mRequestId = sNextReceiveRequestId++;
    mResource = res;
    mCount = count;
    setSingleCity(c);
    setWarningMonths(warningMonths);
    mRequestFinished = showResultMessage;
}

void eFulfillRequestEvent::set(eFulfillRequestEvent &src,
                               const int requestStep,
                               const bool showResultMessage)
{
    setWarningMonths(src.warningMonths());
    mRequestDate = src.mRequestDate;
    mRequestDeadline = src.mRequestDeadline;
    mRequestType = src.mRequestType;
    mRequestResult = src.mRequestResult;
    mResource = src.mResource;
    mCount = src.mCount;
    mCity = src.mCity;
    if (mCity)
    {
        const auto cid = mCity->cityId();
        const int i = static_cast<int>(cid);
        setMinCityId(i);
        setMaxCityId(i);
    }
    mGod = src.mGod;

    mRequestStep = requestStep;
    mRequestId = src.mRequestId;
    mRequestFinished = showResultMessage;
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
    return isPostponedRequestStep(mRequestStep);
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
    auto date = mRequestDeadline;
    if (date == eDate(1, eMonth::january, 1))
    {
        date = mRequestStep <= 1 ? complyDate() : nextDate();
    }
    if (currentDate > date)
        return eLanguage::zeusText(5, 205); // overdue
    return std::to_string(remainingMonths(date, currentDate));
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
