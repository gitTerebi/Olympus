#include "egameevent.h"

#include "engine/egameboard.h"

#include "egodvisitevent.h"
#include "egodattackevent.h"
#include "emonsterunleashedevent.h"
#include "emonsterinvasionevent.h"
#include "einvasionevent.h"
#include "einvasionwarningevent.h"
#include "epaytributeevent.h"
#include "emakerequestevent.h"
#include "ereceiverequestevent.h"
#include "egifttoevent.h"
#include "egiftfromevent.h"
#include "egodquestevent.h"
#include "egodquestfulfilledevent.h"
#include "eplayerconquestevent.h"
#include "eplayerraidevent.h"
#include "eraidresourceevent.h"
#include "earmyreturnevent.h"
#include "emilitarychangeevent.h"
#include "eeconomicchangeevent.h"
#include "etroopsrequestevent.h"
#include "etroopsrequestfulfilledevent.h"
#include "egoddisasterevent.h"
#include "egodtraderesumesevent.h"
#include "erequestaidevent.h"
#include "erequeststrikeevent.h"
#include "erivalarmyawayevent.h"
#include "eearthquakeevent.h"
#include "ecitybecomesevent.h"
#include "etradeshutdownevent.h"
#include "etradeopenupevent.h"
#include "esupplychangeevent.h"
#include "edemandchangeevent.h"
#include "epricechangeevent.h"
#include "ewagechangeevent.h"

eGameEvent::eGameEvent(const eCityId cid,
                       const eGameEventType type,
                       const eGameEventBranch branch,
                       eGameBoard& board) :
    mCid(cid), mType(type), mBranch(branch), mBoard(board) {
    mBoard.addGameEvent(this);
}

eGameEvent::~eGameEvent() {
    mBoard.removeGameEvent(this);
}

stdsptr<eGameEvent> eGameEvent::makeCopy() const {
    const size_t size = 1000000;
    void* mem = malloc(size);
    {
        worldBoard()->setIOIDs();
        eWriteTarget target(mem);
        eWriteStream dst(target);
        dst.writeFormat("eZeus");
        write(dst);
    }
    const auto result = sCreate(mCid, mType, mBranch, mBoard);
    {
        eReadSource source(mem);
        eReadStream src(source);
        src.readFormat();
        result->read(src);
        src.handlePostFuncs();
    }

    free(mem);

    return result;
}

stdsptr<eGameEvent> eGameEvent::sCreate(
        const eCityId cid,
        const eGameEventType type,
        const eGameEventBranch branch,
        eGameBoard& board) {
    switch(type) {
    case eGameEventType::godVisit:
        return e::make_shared<eGodVisitEvent>(cid, branch, board);
    case eGameEventType::godAttack:
        return e::make_shared<eGodAttackEvent>(cid, branch, board);
    case eGameEventType::monsterUnleashed:
        return e::make_shared<eMonsterUnleashedEvent>(cid, branch, board);
    case eGameEventType::monsterInvasion:
        return e::make_shared<eMonsterInvasionEvent>(cid, branch, board);
    case eGameEventType::monsterInvasionWarning:
        return e::make_shared<eMonsterInvasionWarningEvent>(cid, branch, board);
    case eGameEventType::invasion:
        return e::make_shared<eInvasionEvent>(cid, branch, board);
    case eGameEventType::invasionWarning:
        return e::make_shared<eInvasionWarningEvent>(cid, branch, board);
    case eGameEventType::payTribute:
        return e::make_shared<ePayTributeEvent>(cid, branch, board);
    case eGameEventType::makeRequest:
        return e::make_shared<eMakeRequestEvent>(cid, branch, board);
    case eGameEventType::receiveRequest:
        return e::make_shared<eReceiveRequestEvent>(cid, branch, board);
    case eGameEventType::giftTo:
        return e::make_shared<eGiftToEvent>(cid, branch, board);
    case eGameEventType::giftFrom:
        return e::make_shared<eGiftFromEvent>(cid, branch, board);
    case eGameEventType::godQuest:
        return e::make_shared<eGodQuestEvent>(cid, branch, board);
    case eGameEventType::godQuestFulfilled:
        return e::make_shared<eGodQuestFulfilledEvent>(cid, branch, board);
    case eGameEventType::playerConquestEvent:
        return e::make_shared<ePlayerConquestEvent>(cid, branch, board);
    case eGameEventType::raidResourceReceive:
        return e::make_shared<eRaidResourceEvent>(cid, branch, board);
    case eGameEventType::playerRaidEvent:
        return e::make_shared<ePlayerRaidEvent>(cid, branch, board);
    case eGameEventType::armyReturnEvent:
        return e::make_shared<eArmyReturnEvent>(cid, branch, board);
    case eGameEventType::economicChange:
        return e::make_shared<eEconomicChangeEvent>(cid, branch, board);
    case eGameEventType::militaryChange:
        return e::make_shared<eMilitaryChangeEvent>(cid, branch, board);

    case eGameEventType::troopsRequest:
        return e::make_shared<eTroopsRequestEvent>(cid, branch, board);
    case eGameEventType::troopsRequestFulfilled:
        return e::make_shared<eTroopsRequestFulfilledEvent>(cid, branch, board);

    case eGameEventType::godDisaster:
        return e::make_shared<eGodDisasterEvent>(cid, branch, board);
    case eGameEventType::godTradeResumes:
        return e::make_shared<eGodTradeResumesEvent>(cid, branch, board);

    case eGameEventType::requestAid:
        return e::make_shared<eRequestAidEvent>(cid, branch, board);
    case eGameEventType::requestStrike:
        return e::make_shared<eRequestStrikeEvent>(cid, branch, board);

    case eGameEventType::rivalArmyAway:
        return e::make_shared<eRivalArmyAwayEvent>(cid, branch, board);

    case eGameEventType::earthquake:
        return e::make_shared<eEarthquakeEvent>(cid, branch, board);

    case eGameEventType::cityBecomes:
        return e::make_shared<eCityBecomesEvent>(cid, branch, board);

    case eGameEventType::tradeShutdowns:
        return e::make_shared<eTradeShutDownEvent>(cid, branch, board);
    case eGameEventType::tradeOpensUp:
        return e::make_shared<eTradeOpenUpEvent>(cid, branch, board);

    case eGameEventType::supplyChange:
        return e::make_shared<eSupplyChangeEvent>(cid, branch, board);
    case eGameEventType::demandChange:
        return e::make_shared<eDemandChangeEvent>(cid, branch, board);

    case eGameEventType::priceChange:
        return e::make_shared<ePriceChangeEvent>(cid, branch, board);
    case eGameEventType::wageChange:
        return e::make_shared<eWageChangeEvent>(cid, branch, board);
    }
    return nullptr;
}

ePlayerId eGameEvent::playerId() const {
    return mBoard.cityIdToPlayerId(mCid);
}

bool eGameEvent::isOnBoard() const {
    return mBoard.boardCityWithId(mCid);
}

bool eGameEvent::isPersonPlayer() const {
    return mBoard.personPlayer() == playerId();
}

void eGameEvent::setupStartDate(const eDate& currentDate) {
    mStartDate = currentDate;
    mStartDate.nextYears(mDatePlusYears);
    mStartDate.nextMonths(mDatePlusMonths);
    bool nextMonth;
    bool nextYear;
    mStartDate.nextDays(mDatePlusDays, nextMonth, nextYear);
    for(const auto& w : mWarnings) {
        w.second->setStartDate(mStartDate - w.first);
    }
    mNextDate = mStartDate;
}

void eGameEvent::initializeDate(const eDate& startDate,
                                const int period,
                                const int nRuns) {
    setStartDate(startDate);
    setPeriod(period);
    setRepeat(nRuns);
}

void eGameEvent::addWarning(const int daysBefore,
                            const stdsptr<eGameEvent>& event) {
    const auto startDate = mStartDate - daysBefore;
    event->initializeDate(startDate, period(), repeat());
    event->setReason(reason());
    event->mParent = this;
    mWarnings.emplace_back(daysBefore, event);
}

void eGameEvent::clearWarnings() {
    mWarnings.clear();
}

bool eGameEvent::isMainEvent() const {
    return mBranch != eGameEventBranch::child;
}

bool eGameEvent::isRootEvent() const {
    return mBranch == eGameEventBranch::root;
}

bool eGameEvent::isTriggerEvent() const {
    return mBranch == eGameEventBranch::trigger;
}

bool eGameEvent::isChildEvent() const {
    return mBranch == eGameEventBranch::child;
}

void eGameEvent::addConsequence(const stdsptr<eGameEvent>& event) {
    mConsequences.push_back(event);
    if(event->branch() == eGameEventBranch::child) {
        event->setReason(reason());
    }
    event->mParent = this;
}

void eGameEvent::clearConsequences() {
    mConsequences.clear();
}

bool eGameEvent::hasActiveConsequences() const {
    for(const auto& c : mConsequences) {
        if(!c->finished()) return true;
        if(c->hasActiveConsequences()) return true;
    }
    return false;
}

std::string eGameEvent::longDatedName() const {
    auto dateStr = "+" + std::to_string(mDatePlusYears);
    dateStr += "  +" + std::to_string(mDatePlusMonths);
    dateStr += "  +" + std::to_string(mDatePlusDays);
    const auto eventName = longName();
    return dateStr + "  |  " + eventName;
}

void eGameEvent::setReason(const std::string& r) {
    mReason = r;
    for(const auto& w : mWarnings) {
        w.second->setReason(r);
    }
}

void eGameEvent::setStartDate(const eDate& d) {
    mStartDate = d;
    for(const auto& w : mWarnings) {
        w.second->setStartDate(d - w.first);
    }
    mNextDate = mStartDate;
}

void eGameEvent::setPeriod(const int p) {
    mPeriodDays = p;
    for(const auto& w : mWarnings) {
        w.second->setPeriod(p);
    }
}

void eGameEvent::setRepeat(const int r) {
    mTotNRuns = r;
    for(const auto& w : mWarnings) {
        w.second->setRepeat(r);
    }
    mRemNRuns = mTotNRuns;
}

void eGameEvent::handleNewDate(const eDate& date) {
    for(const auto& w : mWarnings) {
        w.second->handleNewDate(date);
    }
    for(const auto& c : mConsequences) {
        c->handleNewDate(date);
    }
    if(mRemNRuns <= 0) return;
    if(date > mNextDate) {
        trigger();
        mRemNRuns--;
        mNextDate += mPeriodDays;
    }
}

int eGameEvent::triggerEventsCount() const {
    int r = 0;
    for(const auto& t : mTriggers) {
        r += t->eventCount();
    }
    return r;
}

eWorldBoard* eGameEvent::worldBoard() const {
    return mBoard.getWorldBoard();
}

void eGameEvent::startingNewEpisode() {
    const bool keepType = mType == eGameEventType::tradeOpensUp ||
                          mType == eGameEventType::supplyChange ||
                          mType == eGameEventType::demandChange ||
                          mType == eGameEventType::priceChange ||
                          mType == eGameEventType::wageChange;
    if(mEpisodeEvent && !keepType) {
        setRepeat(0);
    }
    for(const auto& c : mConsequences) {
        c->startingNewEpisode();
    }
}

void eGameEvent::addTrigger(const stdsptr<eEventTrigger>& et) {
    mTriggers.push_back(et);
}

void eGameEvent::write(eWriteStream& dst) const {
    dst << mIOID;
    dst << mDatePlusDays;
    dst << mDatePlusMonths;
    dst << mDatePlusYears;
    mNextDate.write(dst);
    mStartDate.write(dst);
    dst << mPeriodDays;
    dst << mTotNRuns;
    dst << mRemNRuns;
    dst << mReason;

    for(const auto& w : mWarnings) {
        const auto& event = w.second;
        event->write(dst);
    }

    dst << mConsequences.size();
    for(const auto& c : mConsequences) {
        dst << c->type();
        dst << c->branch();
        c->write(dst);
    }

    for(const auto& et : mTriggers) {
        et->write(dst);
    }

    dst << mEpisodeEvent;
}

void eGameEvent::read(eReadStream& src) {
    src >> mIOID;
    src >> mDatePlusDays;
    src >> mDatePlusMonths;
    src >> mDatePlusYears;
    mNextDate.read(src);
    mStartDate.read(src);
    src >> mPeriodDays;
    src >> mTotNRuns;
    src >> mRemNRuns;
    src >> mReason;

    for(const auto& w : mWarnings) {
        const auto& event = w.second;
        event->read(src);
    }

    int ncs;
    src >> ncs;
    for(int i = 0; i < ncs; i++) {
        eGameEventType type;
        src >> type;
        eGameEventBranch branch;
        src >> branch;
        const auto e = eGameEvent::sCreate(mCid, type, branch, mBoard);
        e->read(src);
        addConsequence(e);
    }

    for(const auto& et : mTriggers) {
        et->read(src);
    }

    src >> mEpisodeEvent;
}
