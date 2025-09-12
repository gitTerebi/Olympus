#ifndef EINVASIONHANDLER_H
#define EINVASIONHANDLER_H

#include <vector>

#include "pointers/estdselfref.h"
#include "pointers/estdpointer.h"

#include "characters/eenlistedforces.h"

class eGameBoard;
class eInvasionEvent;
class eTile;
class eCharacter;
class eReadStream;
class eWriteStream;
class eWorldCity;
class eSoldierBanner;
class ePlayerConquestEvent;
enum class eCityId;
enum class eNationality;

enum class eInvasionStage {
    arrive, spread, wait, march, invade, comeback
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
    eInvasionHandler(eGameBoard& board,
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

    void read(eReadStream& src);
    void write(eWriteStream& dst) const;

    void killAllWithCorpse();

    eTile* currentTile() const { return mCurrentTile; }

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
    void
    generateSoldiersForCity(eTile* const tile,
                            const int infantry,
                            const int cavalry,
                            const int archers,
                            const eCityId cid,
                            const eNationality nat,
                            std::vector<eSoldierBanner*>& solds);

    using eSs = std::vector<std::pair<ePlayerSoldierType, int>>;
    void
    generateSoldiersForCity(eTile* const tile,
                            const eSs& soldTypes,
                            const eCityId cid,
                            std::vector<eSoldierBanner*>& solds);

    void tellHeroesAndGodsToGoBack() const;

    void extractSSFromForces(const eEnlistedForces& forces, eSs& ss) const;

    eGameBoard& mBoard;
    eCityId mTargetCity;
    stdsptr<eWorldCity> mCity;
    stdptr<eInvasionEvent> mEvent;
    stdptr<ePlayerConquestEvent> mConquestEvent;
    eTile* mTile = nullptr;
    eTile* mCurrentTile = nullptr;
    eInvasionStage mStage = eInvasionStage::arrive;
    std::vector<stdsptr<eSoldierBanner>> mBanners;
    std::vector<stdptr<eCharacter>> mHeroesAndGods;

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
    eEnemyBoatFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::enemyBoatFinish) {}
    eEnemyBoatFinish(eGameBoard& board, eCharacter* const c,
                     eInvasionHandler* const invasion) :
        eCharActFunc(board, eCharActFuncType::enemyBoatFinish),
        mCptr(c), mInvasion(invasion) {}

    void call() {
        if(mCptr) mCptr->kill();
        mInvasion->disembark();
    }

    void read(eReadStream& src) {
        src.readCharacter(&board(), [this](eCharacter* const c) {
            mCptr = static_cast<eCharacter*>(c);
        });
        src.readInvasionHandler(&board(), [this](eInvasionHandler* const invasion) {
            mInvasion = invasion;
        });
    }

    void write(eWriteStream& dst) const {
        dst.writeCharacter(mCptr);
        dst.writeInvasionHandler(mInvasion);
    }
private:
    stdptr<eCharacter> mCptr;
    eInvasionHandler* mInvasion = nullptr;
};

#endif // EINVASIONHANDLER_H
