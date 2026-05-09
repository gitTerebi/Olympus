#ifndef EBOARDPLAYER_H
#define EBOARDPLAYER_H

#include "ecityid.h"

#include "edate.h"
#include "edifficulty.h"
#include "egodquest.h"
#include "engine/boardData/ecityfinances.h"

class eGameBoard;
enum class eMonsterType;

class eGodQuestEvent;
class ePlayerConquestEventBase;
class eFulfillRequestEvent;
class ePayTributeEvent;
class eTroopsRequestEvent;
enum class eResourceType;
class eSaveArchive;

class eBoardPlayer {
public:
    eBoardPlayer(const ePlayerId pid, eGameBoard& board);

    ePlayerId id() const { return mId; }
    void setId(const ePlayerId id) { mId = id; }
    eTeamId teamId() const;

    bool isPerson() const;

    void incTime(const int by);
    void nextMonth();

    int drachmas() const { return mDrachmas; }
    void setDrachmas(const int d) { mDrachmas = d; }
    void incDrachmas(const int by, const eFinanceTarget t);

    const eCityFinances& finances() const { return mFinances; }

    eDifficulty difficulty() const { return mDifficulty; }
    void setDifficulty(const eDifficulty d);

    const eDate& inDebtSince() const { return mInDebtSince; }

    using eQuests = std::vector<eGodQuestEvent*>;
    const eQuests& godQuests() const { return mGodQuests; }
    void addGodQuest(eGodQuestEvent* const q);
    void removeGodQuest(eGodQuestEvent* const q);

    using eRequests = std::vector<eFulfillRequestEvent*>;
    const eRequests& cityRequests() const { return mCityRequests; }
    void addCityRequest(eFulfillRequestEvent* const q);
    void removeCityRequest(eFulfillRequestEvent* const q);

    using eTributeRequests = std::vector<ePayTributeEvent*>;
    const eTributeRequests& tributeRequests() const { return mTributeRequests; }
    void addTributeRequest(ePayTributeEvent* const q);
    void removeTributeRequest(ePayTributeEvent* const q);

    using eTroopsRequests = std::vector<eTroopsRequestEvent*>;
    const eTroopsRequests& cityTroopsRequests() const { return mCityTroopsRequests; }
    void addCityTroopsRequest(eTroopsRequestEvent* const q);
    void removeCityTroopsRequest(eTroopsRequestEvent* const q);

    using eConquests = std::vector<ePlayerConquestEventBase*>;
    const eConquests& conquests() const { return mConquests; }
    void addConquest(ePlayerConquestEventBase* const q);
    void removeConquest(ePlayerConquestEventBase* const q);

    const std::vector<eGodQuest>& fulfilledQuests() const
    { return mFulfilledQuests; }
    void addFulfilledQuest(const eGodQuest q);
    const std::vector<eMonsterType>& slayedMonsters() const
    { return mSlayedMonsters; }
    void addSlayedMonster(const eMonsterType m);

    int godAttackTimer() const { return mGodAttackTimer; }
    void resetGodAttackTimer() { mGodAttackTimer = 0; }

    bool askFor(const eResourceType type, const eCityId cid);

    void read(eReadStream& src);
    void write(eWriteStream& dst) const;
private:
    void serialize(eSaveArchive& ar);

    void giftAllies();
    bool askForDrachmas();

    eGameBoard& mBoard;

    ePlayerId mId;

    eDifficulty mDifficulty{eDifficulty::beginner};

    std::vector<eGodQuest> mFulfilledQuests;
    std::vector<eMonsterType> mSlayedMonsters;

    std::vector<eGodQuestEvent*> mGodQuests;
    std::vector<ePlayerConquestEventBase*> mConquests;
    std::vector<eFulfillRequestEvent*> mCityRequests;
    std::vector<ePayTributeEvent*> mTributeRequests;
    std::vector<eTroopsRequestEvent*> mCityTroopsRequests;

    int mDrachmas = 2500;
    eDate mInDebtSince;
    int mLastMonthDrachmas = mDrachmas;
    int mStuckFinanciallyMonths = -1;

    int mGodAttackTimer = 10000000;

    eCityFinances mFinances;
};

#endif // EBOARDPLAYER_H
