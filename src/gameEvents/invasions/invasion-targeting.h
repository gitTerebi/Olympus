#ifndef INVASION_TARGETING_H
#define INVASION_TARGETING_H

#include <vector>

class GameBoard;
class eBuilding;
class SoldierBanner;
enum class eCityId;
enum class eTeamId;

// Augustus-style invasion target selection (see Augustus formation_enemy.c).
// Stateless free functions reused by eInvasionHandler.

// Which class of buildings an invasion force prefers to sack. Picked once per
// invasion, mirrors Augustus attack_type. Serialized as int by the handler, so
// append new values at the end only.
enum class InvasionAttackType {
    food,       // granaries, warehouses, farms
    political,  // palace (the senate equivalent), agoras
    cultural,   // elite/common housing, temples
    military,   // towers, gatehouses, walls, armory
    count
};

namespace InvasionTargeting {

// Best building of the given attack type for the target city: walk the priority
// tiers top-down, and within the first tier that has any attackable building,
// return the one closest to (fromX,fromY). Null if the city has none left.
eBuilding* pickPriorityTarget(GameBoard& board, const eCityId target,
                              const InvasionAttackType type,
                              const int fromX, const int fromY);
eBuilding* pickFallbackTarget(GameBoard& board, const eCityId target,
                              const InvasionAttackType type,
                              const int fromX, const int fromY);

// Nearest defending soldier of the target city to (fromX,fromY), but only if it
// is within maxDist tiles (the Augustus roman_distance proximity gate).
// invaderTid is the attacking force's team, used to tell defenders from invaders.
// Returns false if no defender is close enough.
bool nearestDefender(GameBoard& board, const eCityId target,
                     const eTeamId invaderTid,
                     const int fromX, const int fromY, const int maxDist,
                     int& nX, int& nY);

// True if the invasion force clearly outpowers the target city's garrison, in
// which case invaders ignore defenders and push straight for buildings
// (Augustus enemy_army_is_stronger_than_legions / ignore_roman_soldiers).
bool invadersStrongerThanDefenders(
        GameBoard& board, const eCityId target,
        const std::vector<SoldierBanner*>& invaders);

}

#endif // INVASION_TARGETING_H
