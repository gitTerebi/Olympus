#ifndef INVASION_GENERAL_H
#define INVASION_GENERAL_H

#include <vector>

#include "engine/ecityid.h"
#include "invasion-targeting.h"

class GameBoard;
class SoldierBanner;
class eTile;

class InvasionGeneral {
public:
    InvasionGeneral(GameBoard& board,
                    const eCityId targetCity,
                    const eCityId invadingCity,
                    const InvasionAttackType attackType);

    eTile* chooseTargetTile(const int fromX, const int fromY,
                            const std::vector<SoldierBanner*>& banners) const;
    eTile* moveHalfwayToTarget(eTile* const from,
                               eTile* const target,
                               const std::vector<SoldierBanner*>& banners) const;
    void moveToTarget(eTile* const target,
                      const std::vector<SoldierBanner*>& banners) const;
    bool attackEnemiesNear(const std::vector<SoldierBanner*>& banners) const;

private:
    GameBoard& mBoard;
    eCityId mTargetCity;
    eCityId mInvadingCity;
    InvasionAttackType mAttackType;
};

#endif // INVASION_GENERAL_H
