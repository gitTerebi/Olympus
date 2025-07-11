#ifndef EINVASIONHANDLER_H
#define EINVASIONHANDLER_H

#include <vector>

#include "pointers/estdselfref.h"
#include "pointers/estdpointer.h"

class eGameBoard;
class eInvasionEvent;
class eTile;
class eReadStream;
class eWriteStream;
class eWorldCity;
class eSoldierBanner;
class eEnlistedForces;
enum class eCityId;
enum class eNationality;

enum class eInvasionStage {
    spread, wait, invade, comeback
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

    void initialize(eTile* const tile,
                    const int infantry,
                    const int cavalry,
                    const int archers);

    void initialize(eTile* const tile,
                    const eEnlistedForces& forces);

    void incTime(const int by);

    void read(eReadStream& src);
    void write(eWriteStream& dst) const;

    void killAllWithCorpse();

    eInvasionStage stage() const { return mStage; }
    eTile* tile() const { return mTile; }

    bool nearestSoldier(const int fromX, const int fromY,
                        int& toX,int& toY) const;
private:
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

    eGameBoard& mBoard;
    eCityId mTargetCity;
    stdsptr<eWorldCity> mCity;
    stdptr<eInvasionEvent> mEvent;
    eTile* mTile = nullptr;
    eInvasionStage mStage = eInvasionStage::spread;
    std::vector<stdsptr<eSoldierBanner>> mBanners;

    int mWait = 0;
};

#endif // EINVASIONHANDLER_H
