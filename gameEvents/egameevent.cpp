#include "egameevent.h"

#include "engine/e-game-board.h"
#include "fileIO/esavearchive.h"

#include "gameEvents/gods/egodvisitevent.h"
#include "gameEvents/gods/egodattackevent.h"
#include "gameEvents/invasions/monster-unleashed-event.h"
#include "gameEvents/invasions/monster-invasion-event.h"
#include "gameEvents/invasions/monster-in-city-event.h"
#include "gameEvents/invasions/invasion-event.h"
#include "ereceivetributeevent.h"
#include "requests/e-pay-tribute-event.h"
#include "emakerequestevent.h"
#include "requests/e-fulfill-request-event.h"
#include "egifttoevent.h"
#include "egiftfromevent.h"
#include "gameEvents/gods/egodquestevent.h"
#include "gameEvents/gods/egodquestfulfilledevent.h"
#include "eplayerconquestevent.h"
#include "eplayerraidevent.h"
#include "eraidresourceevent.h"
#include "earmyreturnevent.h"
#include "emilitarychangeevent.h"
#include "eeconomicchangeevent.h"
#include "etroopsrequestevent.h"
#include "etroopsrequestfulfilledevent.h"
#include "gameEvents/gods/egoddisasterevent.h"
#include "gameEvents/gods/egodtraderesumesevent.h"
#include "erequestaidevent.h"
#include "erequeststrikeevent.h"
#include "erivalarmyawayevent.h"
#include "eearthquakeevent.h"
#include "elavaevent.h"
#include "etidalwaveevent.h"
#include "esinklandevent.h"
#include "elandslideevent.h"
#include "ecitybecomesevent.h"
#include "etradeshutdownevent.h"
#include "etradeopenupevent.h"
#include "esupplychangeevent.h"
#include "edemandchangeevent.h"
#include "epricechangeevent.h"
#include "ewagechangeevent.h"
#include "ereinforcementsevent.h"

eGameEvent::eGameEvent(const eCityId cid,
                       const eGameEventType type,
                       const eGameEventBranch branch,
                       eGameBoard& board) :
    mCid(cid), mType(type), mBranch(branch), mBoard(board) {
    mBoard.addGameEvent(this);

    const auto e4 = eLanguage::text("base_trigger");
    mBaseTrigger = e::make_shared<eEventTrigger>(cid, e4, board);
    addTrigger(mBaseTrigger);
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
    if(!result) {
        free(mem);
        return nullptr;
    }
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
        return nullptr;
    case eGameEventType::monsterInCity:
        return e::make_shared<eMonsterInCityEvent>(cid, branch, board);
    case eGameEventType::invasion:
        return e::make_shared<eInvasionEvent>(cid, branch, board);
    case eGameEventType::invasionWarning:
        return nullptr;
    case eGameEventType::receiveTribute:
        return e::make_shared<eReceiveTributeEvent>(cid, branch, board);
    case eGameEventType::payTribute:
        return e::make_shared<ePayTributeEvent>(cid, branch, board);
    case eGameEventType::makeRequest:
        return e::make_shared<eMakeRequestEvent>(cid, branch, board);
    case eGameEventType::receiveRequest:
        return e::make_shared<eFulfillRequestEvent>(cid, branch, board);
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
    case eGameEventType::lavaFlow:
        return e::make_shared<eLavaEvent>(cid, branch, board);
    case eGameEventType::tidalWave:
        return e::make_shared<eTidalWaveEvent>(cid, branch, board);
    case eGameEventType::sinkLand:
        return e::make_shared<eSinkLandEvent>(cid, branch, board);
    case eGameEventType::landSlide:
        return e::make_shared<eLandSlideEvent>(cid, branch, board);

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

    case eGameEventType::reinforcementsEvent:
        return e::make_shared<eReinforcementsEvent>(cid, branch, board);
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
    mNextDate = currentDate;
    const int years = chooseYear();
    mNextDate.nextYears(years);
    mNextDate.nextMonths(mDatePlusMonths);
    bool nextMonth;
    bool nextYear;
    mNextDate.nextDays(mDatePlusDays, nextMonth, nextYear);
    updateWarningDates();
}

void eGameEvent::fastForward(const eDate& date) {
    for(const auto& c : mConsequences) {
        c->fastForward(date);
    }
    while(mRemNRuns > 0 && date > mNextDate) {
        mRemNRuns--;
        mNextDate += choosePeriod();
    }
    updateWarningDates();
}

void eGameEvent::initializeDate(const eDate& startDate,
                                const int period,
                                const int nRuns) {
    mNextDate = startDate;
    updateWarningDates();
    setPeriodMin(period);
    setPeriodMax(period);
    setRepeat(nRuns);
}

void eGameEvent::addWarning(const stdsptr<eWarning>& w) {
    mWarnings.emplace_back(w);
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
    auto dateStr = "+" + std::to_string(mDatePlusYearsMin) +
                   "-" + std::to_string(mDatePlusYearsMax);
    dateStr += "  +" + std::to_string(mDatePlusMonths);
    dateStr += "  +" + std::to_string(mDatePlusDays);
    const auto eventName = longName();
    return dateStr + "  |  " + eventName;
}

void eGameEvent::setReason(const std::string& r) {
    mReason = r;
}

void eGameEvent::setWarningMonths(const int ms) {
    mWarningMonths = ms;
}

int eGameEvent::choosePeriod() const {
    int periodDays = mPeriodDaysMin;
    if(mPeriodDaysMax > mPeriodDaysMin) {
        periodDays += eRand::rand() % (mPeriodDaysMax - mPeriodDaysMin);
    }
    return periodDays;
}

int eGameEvent::chooseYear() const {
    int years = mDatePlusYearsMin;
    if(mDatePlusYearsMax > mDatePlusYearsMin) {
        years += eRand::rand() % (mDatePlusYearsMax - mDatePlusYearsMin);
    }
    return years;
}

void eGameEvent::setRepeat(const int r) {
    mRemNRuns = r;
}

void eGameEvent::handleNewDate(const eDate& date) {
    if(mEpisodeCompleteEvent) return;
    for(const auto& c : mConsequences) {
        c->handleNewDate(date);
    }
    if(mRemNRuns <= 0) return;
    for(const auto& w : mWarnings) {
        w->handleNewDate(date);
    }
    if(date > mNextDate) {
        trigger();
        mRemNRuns--;
        const int periodDays = choosePeriod();
        mNextDate += periodDays;
        updateWarningDates();
        callBaseTrigger();
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
    return &mBoard.world();
}

void eGameEvent::startingNewEpisode() {
    if(mEpisodeEvent) {
        setRepeat(0);
    }
    for(const auto& c : mConsequences) {
        c->startingNewEpisode();
    }
}

void eGameEvent::addTrigger(const stdsptr<eEventTrigger>& et) {
    mTriggers.push_back(et);
}

void eGameEvent::callBaseTrigger() {
    const auto board = gameBoard();
    if(!board) return;
    const auto date = board->date();
    mBaseTrigger->trigger(*this, date, "");
}

void eGameEvent::updateWarningDates() {
    const auto& board = *gameBoard();
    const auto currentDate = board.date();
    for(const auto& w : mWarnings) {
        const int ms = w->warningMonths();
        if(ms > mWarningMonths ||
           (ms == mWarningMonths &&
            !w->isInitialWarning())) {
            w->setFinished(true);
            continue;
        }
        auto wdate = mNextDate;
        wdate.prevMonths(ms);
        w->setNextDate(wdate);
        if(wdate <= currentDate) {
            if(wdate == currentDate) {
                w->trigger();
            }
            w->setFinished(true);
        }
    }
}

void eGameEvent::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eGameEvent*>(this)->serialize(ar);
}

void eGameEvent::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}

void eGameEvent::serialize(eSaveArchive& ar) {
    ar.field("mIOID", mIOID);
    ar.field("mDatePlusDays", mDatePlusDays);
    ar.field("mDatePlusMonths", mDatePlusMonths);
    ar.field("mDatePlusYearsMin", mDatePlusYearsMin);
    ar.field("mDatePlusYearsMax", mDatePlusYearsMax);
    if(ar.reading()) mNextDate.read(ar.readStream());
    else mNextDate.write(ar.writeStream());
    ar.field("mPeriodDaysMin", mPeriodDaysMin);
    ar.field("mPeriodDaysMax", mPeriodDaysMax);
    ar.field("mWarningMonths", mWarningMonths);
    ar.field("mRemNRuns", mRemNRuns);
    ar.field("mReason", mReason);
    ar.field("mEpisodeCompleteEvent", mEpisodeCompleteEvent);

    ar.fixedArrayField("warnings.count", mWarnings, [](eSaveArchive& ar, auto& w) {
        ar.object(w);
    });

    ar.arrayField("consequences", mConsequences, [this](eSaveArchive& ar, auto& e) {
        eGameEventType type;
        eGameEventBranch branch;
        if(ar.writing()) {
            type = e->type();
            branch = e->branch();
        }
        ar.field("type", type);
        ar.field("branch", branch);
        if(ar.writing()) {
            ar.object(e);
        } else {
            e = eGameEvent::sCreate(mCid, type, branch, mBoard);
            ar.object(e);
            if(e->branch() == eGameEventBranch::child) {
                e->setReason(reason());
            }
            e->mParent = this;
        }
    });

    ar.fixedArrayField("triggers.count", mTriggers, [](eSaveArchive& ar, auto& et) {
        ar.object(et);
    });

    ar.field("mEpisodeEvent", mEpisodeEvent);
}

void eGameEvent::loadResources() const {
    for(const auto& e : mConsequences) {
        e->loadResources();
    }
    for(const auto& et : mTriggers) {
        et->loadResources();
    }
}

void eGameEvent::respond(int, eCityId) {}
