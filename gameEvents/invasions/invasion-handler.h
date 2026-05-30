#ifndef INVASION_HANDLER_H
#define INVASION_HANDLER_H

#include <vector>

#include "pointers/estdselfref.h"
#include "pointers/estdpointer.h"

#include "characters/eenlistedforces.h"
#include "invasion-general.h"

class GameBoard;
class eInvasionEvent;
class eTile;
class eCharacter;
class eReadStream;
class eWriteStream;
class eWorldCity;
class SoldierBanner;
class ePlayerConquestEvent;
class eSaveArchive;
enum class eCityId;
enum class eNationality;

enum class eInvasionStage {
    arrive, active, comeback
};

enum class ePlayerSoldierType {
    greekHoplite,
    greekHorseman,
    greekRockthrower,

    atlanteanHoplite,
    atlanteanArcher,
    atlanteanChariot,

    aresWarrior,
    amazon
};

class eInvasionHandler {
public:
    eInvasionHandler(GameBoard& board,
                     const eCityId targetCity,
                     const stdsptr<eWorldCity>& city,
                     eInvasionEvent* const event);
    ~eInvasionHandler();

    void disembark();

    void initializeSeaInvasion(eTile* const waterTile,
                               eTile* const disembarkTile,
                               eTile* const shoreTile,
                               const int infantry,
                               const int cavalry,
                               const int archers);

    void initializeSeaInvasion(eTile* const waterTile,
                               eTile* const disembarkTile,
                               eTile* const shoreTile,
                               const eEnlistedForces& forces,
                               ePlayerConquestEvent* const conquestEvent);

    void initializeLandInvasion(eTile* const tile,
                                const int infantry,
                                const int cavalry,
                                const int archers);

    void initializeLandInvasion(eTile* const tile,
                                const eEnlistedForces& forces,
                                ePlayerConquestEvent* const conquestEvent);

    void incTime(const int by);

    void serialize(eSaveArchive& ar);

    void killAllWithCorpse();

    eTile* currentTile() const { return mGState.fCurrentTile; }
    eTile* moveFromTile() const { return mGState.fMoveFrom; }
    eTile* moveToTile() const { return mGState.fMoveTo; }
    eTile* generalTargetTile() const { return mGState.fTargetTile; }

    eInvasionStage stage() const { return mStage; }
    eTile* tile() const { return mTile; }

    bool nearestSoldier(const int fromX, const int fromY,
                        int& toX,int& toY) const;

    void setIOID(const int id) { mIOID = id; }
    int ioID() const { return mIOID; }
private:
    void initializeBoats(eTile* const tile, const int troops);
    void spawnBoat();

    void
    generateImmortals(eTile* const tile, const eCityId cid,
                      const bool ares, const std::vector<eHeroType>& heroes);
    bool immortalsFighting() const;

    void
    generateSoldiersForCity(eTile* const tile,
                            const int infantry,
                            const int cavalry,
                            const int archers,
                            const eCityId cid,
                            const eNationality nat,
                            std::vector<SoldierBanner*>& solds);

    using eSs = std::vector<std::pair<ePlayerSoldierType, int>>;
    void
    generateSoldiersForCity(eTile* const tile,
                            const eSs& soldTypes,
                            const eCityId cid,
                            std::vector<SoldierBanner*>& solds);

    void tellHeroesAndGodsToGoBack() const;

    // Facing/line for a formation spawned at (fromX,fromY) so it points toward
    // the target city centre (provisional aim until the general picks a target).
    void spawnFacingTowardTarget(int fromX, int fromY,
                                 int& facing, int& lineDX, int& lineDY) const;

    void extractSSFromForces(const eEnlistedForces& forces, eSs& ss) const;

    GameBoard& mBoard;
    eCityId mTargetCity;
    stdsptr<eWorldCity> mCity;
    stdptr<eInvasionEvent> mEvent;
    stdptr<ePlayerConquestEvent> mConquestEvent;
    eTile* mTile = nullptr;
    eInvasionStage mStage = eInvasionStage::arrive;
    InvasionAttackType mAttackType = InvasionAttackType::food;
    std::vector<stdsptr<SoldierBanner>> mBanners;
    std::vector<stdptr<eCharacter>> mHeroesAndGods;

    // Campaign state the general drives (it stays stateless and reads/writes
    // this through advance()). Serialized here so it persists across save/load.
    eGeneralState mGState;

    int mWait = 0;

    int mInfantryLeft = 0;
    int mCavalryLeft = 0;
    int mArchersLeft = 0;

    eSs mForcesLeft;

    struct eAllyForces {
        eCityId fCid;
        eNationality fNat;
        int fInfantryLeft = 0;
        int fCavalryLeft = 0;
        int fArchersLeft = 0;
    };

    std::vector<eAllyForces> mAllyForcesLeft;

    bool mAresLeft = false;
    std::vector<eHeroType> mHeroesLeft;

    eTile* mBoatsTile = nullptr;
    eTile* mDisembarkTile = nullptr;
    int mBoatsLeft = 0;
    std::vector<stdptr<eCharacter>> mBoats;

    const int mSoldiersPerBoat = 4*8;

    int mIOID = -1;
};

class eEnemyBoatFinish : public eCharActFunc {
public:
    eEnemyBoatFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::enemyBoatFinish) {}
    eEnemyBoatFinish(GameBoard& board, eCharacter* const c,
                     eInvasionHandler* const invasion) :
        eCharActFunc(board, eCharActFuncType::enemyBoatFinish),
        mCptr(c), mInvasion(invasion) {}

    void call() override {
        if(mCptr) mCptr->kill();
        mInvasion->disembark();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterField("character", &board(), mCptr);
        ar.invasionHandlerField("invasion", &board(), mInvasion);
    }
private:
    stdptr<eCharacter> mCptr;
    eInvasionHandler* mInvasion = nullptr;
};

#endif // INVASION_HANDLER_H
