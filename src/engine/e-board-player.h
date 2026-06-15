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
class PlayerConquestEventBase;
class SendResourcesToCityEvent;
class ReceiveTributeEvent;
class SendTroopsEvent;
enum class eResourceType;
class SaveArchive;

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

    using eRequests = std::vector<SendResourcesToCityEvent*>;
    const eRequests& cityRequests() const { return mCityRequests; }
    void addCityRequest(SendResourcesToCityEvent* const q);
    void removeCityRequest(SendResourcesToCityEvent* const q);

    using eTributeRequests = std::vector<ReceiveTributeEvent*>;
    const eTributeRequests& tributeRequests() const { return mTributeRequests; }
    void addTributeRequest(ReceiveTributeEvent* const q);
    void removeTributeRequest(ReceiveTributeEvent* const q);

    using eTroopsRequests = std::vector<SendTroopsEvent*>;
    const eTroopsRequests& cityTroopsRequests() const { return mCityTroopsRequests; }
    void addCityTroopsRequest(SendTroopsEvent* const q);
    void removeCityTroopsRequest(SendTroopsEvent* const q);

    using eConquests = std::vector<PlayerConquestEventBase*>;
    const eConquests& conquests() const { return mConquests; }
    void addConquest(PlayerConquestEventBase* const q);
    void removeConquest(PlayerConquestEventBase* const q);

    const std::vector<eGodQuest>& fulfilledQuests() const
    { return mFulfilledQuests; }
    void addFulfilledQuest(const eGodQuest q);
    const std::vector<eMonsterType>& slayedMonsters() const
    { return mSlayedMonsters; }
    void addSlayedMonster(const eMonsterType m);

    int godAttackTimer() const { return mGodAttackTimer; }
    void resetGodAttackTimer() { mGodAttackTimer = 0; }

    bool askFor(const eResourceType type, const eCityId cid);

    void serialize(SaveArchive& ar);
private:
    void giftAllies();
    bool askForDrachmas();

    GameBoard& mBoard;

    ePlayerId mId;

    Difficulty mDifficulty{Difficulty::beginner};

    std::vector<eGodQuest> mFulfilledQuests;
    std::vector<eMonsterType> mSlayedMonsters;

    std::vector<eGodQuestEvent*> mGodQuests;
    std::vector<PlayerConquestEventBase*> mConquests;
    std::vector<SendResourcesToCityEvent*> mCityRequests;
    std::vector<ReceiveTributeEvent*> mTributeRequests;
    std::vector<SendTroopsEvent*> mCityTroopsRequests;

    int mDrachmas = 2500;
    eDate mInDebtSince;
    int mLastMonthDrachmas = mDrachmas;
    int mStuckFinanciallyMonths = -1;

    int mGodAttackTimer = 10000000;

    eCityFinances mFinances;
};

#endif // EBOARDPLAYER_H
