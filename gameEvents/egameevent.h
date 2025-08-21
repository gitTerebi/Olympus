#ifndef EGAMEEVENT_H
#define EGAMEEVENT_H

#include "pointers/eobject.h"

#include "engine/edate.h"
#include "eeventtrigger.h"
#include "engine/ecityid.h"
#include "ewarning.h"

class eWriteStream;
class eReadStream;
class eWorldBoard;

enum class eGameEventType {
    godVisit,
    godAttack,
    monsterUnleashed,
    monsterInvasion,
    monsterInvasionWarning, // old, unused
    invasion,
    invasionWarning, // old, unused
    payTribute,
    receiveRequest,
    makeRequest,
    giftTo,
    giftFrom,
    godQuest,
    godQuestFulfilled,
    playerConquestEvent,
    playerRaidEvent,
    raidResourceReceive,
    armyReturnEvent,

    militaryChange,
    economicChange,

    troopsRequest,
    troopsRequestFulfilled,

    godDisaster,
    godTradeResumes,

    requestAid,
    requestStrike,
    rivalArmyAway,

    earthquake,

    cityBecomes,

    tradeShutdowns,
    tradeOpensUp,

    supplyChange,
    demandChange,

    priceChange,

    wageChange,

    monsterInCity,
    tidalWave
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
               eGameBoard& board);
    ~eGameEvent();

    virtual void trigger() = 0;

    virtual std::string longName() const = 0;

    stdsptr<eGameEvent> makeCopy() const;

    virtual void write(eWriteStream& dst) const;
    virtual void read(eReadStream& src);

    static stdsptr<eGameEvent> sCreate(const eCityId cid,
                                       const eGameEventType type,
                                       const eGameEventBranch branch,
                                       eGameBoard& board);

    eGameEventType type() const { return mType; }

    void setIOID(const int id) { mIOID = id; }
    int ioID() const { return mIOID; }

    eCityId cityId() const { return mCid; }
    ePlayerId playerId() const;
    bool isOnBoard() const;
    bool isPersonPlayer() const;

    void setupStartDate(const eDate& currentDate);

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

    int choosePeriod() const;
    int chooseYear() const;

    int repeat() const { return mTotNRuns; }
    void setRepeat(const int r);

    void handleNewDate(const eDate& date);
    virtual bool finished() const { return mRemNRuns <= 0; }

    const std::vector<stdsptr<eWarning>>& warnings() const
    { return mWarnings; }

    const std::vector<stdsptr<eEventTrigger>>& triggers() const
    { return mTriggers; }
    int triggerEventsCount() const;

    eGameEvent* parent() const { return mParent; }

    eGameBoard* gameBoard() const { return &mBoard; }
    eWorldBoard* worldBoard() const;

    void startingNewEpisode();

    bool episodeEvent() const { return mEpisodeEvent; }
    void setIsEpisodeEvent(const bool e) { mEpisodeEvent = e; }
protected:
    void addTrigger(const stdsptr<eEventTrigger>& et);
private:
    void updateWarningDates();

    const eCityId mCid;
    const eGameEventType mType;
    const eGameEventBranch mBranch;
    eGameBoard& mBoard;

    bool mEpisodeEvent = false;
    eWorldBoard* mWorldBoard = nullptr;

    stdptr<eGameEvent> mParent;

    std::vector<stdsptr<eGameEvent>> mConsequences;
    std::vector<stdsptr<eWarning>> mWarnings;
    std::vector<stdsptr<eEventTrigger>> mTriggers;

    std::string mReason;

    int mDatePlusDays = 15;
    int mDatePlusMonths = 2;
    int mDatePlusYearsMin = 3;
    int mDatePlusYearsMax = 3;
    int mPeriodDaysMin = 100;
    int mPeriodDaysMax = 100;
    int mTotNRuns = 1;

    int mRemNRuns = 0;
    eDate mNextDate{1, eMonth::january, 1};

    int mIOID = -1;
};

#endif // EGAMEEVENT_H
