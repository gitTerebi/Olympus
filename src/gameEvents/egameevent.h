#ifndef EGAMEEVENT_H
#define EGAMEEVENT_H

#include "pointers/eobject.h"

#include "engine/edate.h"
#include "eeventtrigger.h"
#include "engine/ecityid.h"
#include "ewarning.h"

class eWriteStream;
class eReadStream;
class eSaveArchive;
class WorldBoard;
struct eEventData;

enum class eGameEventType {
    godVisit = 0,
    godAttack = 1,
    monsterUnleashed = 2,
    monsterInvasion = 3,
    monsterInvasionWarning = 4, // old, unused
    invasion = 5,
    invasionWarning = 6, // old, unused
    payTribute = 7,
    sendResourcesToCity = 8,
    receiveRequestedResources = 9,
    giftTo = 10,
    giftFrom = 11,
    godQuest = 12,
    godQuestFulfilled = 13,
    playerConquestEvent = 14,
    playerRaidEvent = 15,
    raidResourceReceive = 16,
    armyReturnEvent = 17,

    militaryChange = 18,
    economicChange = 19,

    sendTroops = 20,
    troopsSent = 21,

    godDisaster = 22,
    godTradeResumes = 23,

    askForAid = 24,
    askForStrike = 25,
    rivalArmyAway = 26,

    earthquake = 27,

    cityBecomes = 28,

    tradeShutdowns = 29,
    tradeOpensUp = 30,

    supplyChange = 31,
    demandChange = 32,

    priceChange = 33,

    wageChange = 34,

    monsterInCity = 35,
    tidalWave = 36,
    lavaFlow = 37,
    sinkLand = 38,
    landSlide = 39,

    reinforcementsEvent = 40,
    receiveTribute = 41
};

enum class eGameEventBranch {
    root,
    child,
    trigger
};

class eGameEvent : public eStdSelfRef {
public:
    eGameEvent(const eCityId cid,
               const eGameEventType type,
               const eGameEventBranch branch,
               GameBoard& board);
    ~eGameEvent();

    virtual void trigger() = 0;

    virtual std::string longName() const = 0;

    stdsptr<eGameEvent> makeCopy() const;

    virtual void loadResources() const;
    virtual void respond(int response, eCityId city = eCityId::neutralAggresive);
    virtual void fillEventDataActions(eEventData& ed);

    static stdsptr<eGameEvent> sCreate(const eCityId cid,
                                       const eGameEventType type,
                                       const eGameEventBranch branch,
                                       GameBoard& board);

    eGameEventType type() const { return mType; }

    void setIOID(const int id) { mIOID = id; }
    int ioID() const { return mIOID; }
    void setRuntimeId(const int id) { mRuntimeId = id; }
    int runtimeId() const { return mRuntimeId; }

    eCityId cityId() const { return mCid; }
    ePlayerId playerId() const;
    bool isOnBoard() const;
    bool isPersonPlayer() const;

    void setupStartDate(const eDate& currentDate);
    void fastForward(const eDate& date);

    void initializeDate(const eDate& startDate,
                        const int period = 0,
                        const int nRuns = 1);

    bool isMainEvent() const;
    bool isRootEvent() const;
    bool isTriggerEvent() const;
    bool isChildEvent() const;
    eGameEventBranch branch() const { return mBranch; }

    void addWarning(const stdsptr<eWarning> &w);
    void clearWarnings();

    void addConsequence(const stdsptr<eGameEvent>& event);
    void clearConsequences();
    bool hasActiveConsequences() const;

    template <typename T = eGameEvent>
    T* rootEvent() {
        if(isRootEvent()) return static_cast<T*>(this);
        if(mParent) return mParent->rootEvent<T>();
        return nullptr;
    }

    template <typename T = eGameEvent>
    T* mainEvent() {
        if(isMainEvent()) return static_cast<T*>(this);
        if(mParent) return mParent->mainEvent<T>();
        return nullptr;
    }

    std::string longDatedName() const;

    void setReason(const std::string& r);
    const std::string& reason() const { return mReason; }

    const eDate& nextDate() const { return mNextDate; }

    void setEpisodeCompleteEvent(const bool c) { mEpisodeCompleteEvent = c; }
    bool episodeCompleteEvent() const { return mEpisodeCompleteEvent; }

    int datePlusDays() const { return mDatePlusDays; }
    void setDatePlusDays(const int d) { mDatePlusDays = d; }
    int datePlusMonths() const { return mDatePlusMonths; }
    void setDatePlusMonths(const int m) { mDatePlusMonths = m; }

    int datePlusYearsMin() const { return mDatePlusYearsMin; }
    void setDatePlusYearsMin(const int y) { mDatePlusYearsMin = y; }

    int datePlusYearsMax() const { return mDatePlusYearsMax; }
    void setDatePlusYearsMax(const int y) { mDatePlusYearsMax = y; }

    int periodMin() const { return mPeriodDaysMin; }
    void setPeriodMin(const int p) { mPeriodDaysMin = p; }

    int periodMax() const { return mPeriodDaysMax; }
    void setPeriodMax(const int p) { mPeriodDaysMax = p; }

    virtual void setWarningMonths(const int ms);
    int warningMonths() const { return mWarningMonths; }

    int choosePeriod() const;
    int chooseYear() const;

    int repeat() const { return mRemNRuns; }
    void setRepeat(const int r);

    void handleNewDate(const eDate& date);
    virtual bool finished() const { return mRemNRuns <= 0; }

    const std::vector<stdsptr<eWarning>>& warnings() const
    { return mWarnings; }

    const std::vector<stdsptr<eEventTrigger>>& triggers() const
    { return mTriggers; }
    int triggerEventsCount() const;

    eGameEvent* parent() const { return mParent; }

    GameBoard* gameBoard() const { return &mBoard; }
    WorldBoard* worldBoard() const;

    void startingNewEpisode();

    bool episodeEvent() const { return mEpisodeEvent; }
    void setIsEpisodeEvent(const bool e) { mEpisodeEvent = e; }

    eEventTrigger& baseTrigger() { return *mBaseTrigger; }

    void serialize(eSaveArchive& ar);
protected:
    void addTrigger(const stdsptr<eEventTrigger>& et);
    void callBaseTrigger();
    virtual void serializeFields(eSaveArchive& ar);

    int mDatePlusDays = 0;
    int mDatePlusMonths = 0;
    int mDatePlusYearsMin = 0;
    int mDatePlusYearsMax = 0;
    int mPeriodDaysMin = 100;
    int mPeriodDaysMax = 100;

    int mRemNRuns = 0;
    eDate mNextDate{1, eMonth::january, 1};
private:
    bool shouldHaveBaseTrigger() const;
    void updateWarningDates();

    const eCityId mCid;
    const eGameEventType mType;
    const eGameEventBranch mBranch;
    GameBoard& mBoard;

    bool mEpisodeEvent = false;
    WorldBoard* mWorldBoard = nullptr;

    stdptr<eGameEvent> mParent;

    std::vector<stdsptr<eGameEvent>> mConsequences;
    std::vector<stdsptr<eWarning>> mWarnings;
    std::vector<stdsptr<eEventTrigger>> mTriggers;
    stdsptr<eEventTrigger> mBaseTrigger;

    std::string mReason;

    bool mEpisodeCompleteEvent = false;

    int mWarningMonths = 2;

    int mIOID = -1;
    int mRuntimeId = -1;
};

#endif // EGAMEEVENT_H
