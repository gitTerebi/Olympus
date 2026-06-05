#ifndef EBOARDPLAYER_H
#define EBOARDPLAYER_H

#include "ecityid.h"

#include "edate.h"
#include "difficulty.h"
#include "engine/egodquest.h"
#include "engine/boardData/ecityfinances.h"

class GameBoard;
enum class eMonsterType;

class eGodQuestEvent;
class ePlayerConquestEventBase;
class FulfillRequestEvent;
class GetTributeEvent;
class eTroopsRequestEvent;
enum class eResourceType;
class eSaveArchive;

class eBoardPlayer {
public:
    eBoardPlayer(const ePlayerId pid, GameBoard& board);

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

    Difficulty difficulty() const { return mDifficulty; }
    void setDifficulty(const Difficulty d);

    const eDate& inDebtSince() const { return mInDebtSince; }

    using eQuests = std::vector<eGodQuestEvent*>;
    const eQuests& godQuests() const { return mGodQuests; }
    void addGodQuest(eGodQuestEvent* const q);
    void removeGodQuest(eGodQuestEvent* const q);

    using eRequests = std::vector<FulfillRequestEvent*>;
    const eRequests& cityRequests() const { return mCityRequests; }
    void addCityRequest(FulfillRequestEvent* const q);
    void removeCityRequest(FulfillRequestEvent* const q);

    using eTributeRequests = std::vector<GetTributeEvent*>;
    const eTributeRequests& tributeRequests() const { return mTributeRequests; }
    void addTributeRequest(GetTributeEvent* const q);
    void removeTributeRequest(GetTributeEvent* const q);

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

    void serialize(eSaveArchive& ar);
private:
    void giftAllies();
    bool askForDrachmas();

    GameBoard& mBoard;

    ePlayerId mId;

    Difficulty mDifficulty{Difficulty::beginner};

    std::vector<eGodQuest> mFulfilledQuests;
    std::vector<eMonsterType> mSlayedMonsters;

    std::vector<eGodQuestEvent*> mGodQuests;
    std::vector<ePlayerConquestEventBase*> mConquests;
    std::vector<FulfillRequestEvent*> mCityRequests;
    std::vector<GetTributeEvent*> mTributeRequests;
    std::vector<eTroopsRequestEvent*> mCityTroopsRequests;

    int mDrachmas = 2500;
    eDate mInDebtSince;
    int mLastMonthDrachmas = mDrachmas;
    int mStuckFinanciallyMonths = -1;

    int mGodAttackTimer = 10000000;

    eCityFinances mFinances;
};

#endif // EBOARDPLAYER_H
