#include "invasion-targeting.h"

#include <cmath>

#include "engine/game-board.h"
#include "engine/board-city.h"
#include "engine/etile.h"
#include "buildings/ebuilding.h"
#include "characters/soldier-banner.h"

namespace {

// Priority tiers, top-down, mapping Augustus ENEMY_ATTACK_PRIORITY onto eZeus
// building types. First tier with an attackable building wins; closest in that
// tier is chosen. Terminated by eBuildingType::none.
const eBuildingType kFood[] = {
    eBuildingType::granary,
    eBuildingType::wheatFarm, eBuildingType::carrotsFarm,
    eBuildingType::onionsFarm, eBuildingType::growersLodge,
    eBuildingType::huntingLodge, eBuildingType::none
};

// Political is the conquest attack type: raze the city's political buildings
// first, then the palace LAST. Killing the palace auto-conquers the city (the
// handler turns the campaign into invadersWon), so it must be the final target,
// not the first, or the city would fall before the rest is sacked.
const eBuildingType kPolitical[] = {
    eBuildingType::grandAgora, eBuildingType::commonAgora,
    eBuildingType::palace, eBuildingType::none
};

const eBuildingType kCultural[] = {
    eBuildingType::gymnasium, eBuildingType::college,
    eBuildingType::dramaSchool, eBuildingType::podium,
    eBuildingType::bibliotheke, eBuildingType::observatory,
    eBuildingType::university, eBuildingType::laboratory,
    eBuildingType::inventorsWorkshop, eBuildingType::museum,
    eBuildingType::none
};

const eBuildingType kMilitary[] = {
    eBuildingType::tower, eBuildingType::gatehouse, eBuildingType::wall,
    eBuildingType::armory, eBuildingType::maintenanceOffice,
    eBuildingType::none
};

const eBuildingType* tiersFor(const InvasionAttackType type) {
    switch(type) {
    case InvasionAttackType::food:      return kFood;
    case InvasionAttackType::political: return kPolitical;
    case InvasionAttackType::cultural:  return kCultural;
    case InvasionAttackType::military:  return kMilitary;
    default:                            return kFood;
    }
}

// Closest attackable, in-use building of exactly `bt` in the target city.
eBuilding* closestOfType(GameBoard& board, const eCityId target,
                         const eBuildingType bt,
                         const int fromX, const int fromY) {
    if(!eBuilding::sAttackable(bt)) return nullptr;
    eBuilding* best = nullptr;
    int bestDist = 0;
    for(const auto b : board.buildings()) {
        if(b->type() != bt) continue;
        if(b->cityId() != target) continue;
        if(!b->enabled()) continue;
        const auto t = b->centerTile();
        if(!t) continue;
        const int dx = fromX - t->x();
        const int dy = fromY - t->y();
        const int dist = dx*dx + dy*dy;
        if(!best || dist < bestDist) {
            best = b;
            bestDist = dist;
        }
    }
    return best;
}

}

namespace InvasionTargeting {

eBuilding* pickPriorityTarget(GameBoard& board, const eCityId target,
                              const InvasionAttackType type,
                              const int fromX, const int fromY) {
    const eBuildingType* tiers = tiersFor(type);
    for(int i = 0; tiers[i] != eBuildingType::none; i++) {
        const auto b = closestOfType(board, target, tiers[i], fromX, fromY);
        if(b) return b;
    }
    return nullptr;
}

eBuilding* pickFallbackTarget(GameBoard& board, const eCityId target,
                              const InvasionAttackType type,
                              const int fromX, const int fromY) {
    const InvasionAttackType fallbacks[] = {
        InvasionAttackType::food,
        InvasionAttackType::cultural,
        InvasionAttackType::military,
        InvasionAttackType::political
    };
    for(const auto fallback : fallbacks) {
        if(fallback == type) continue;
        const auto tiers = tiersFor(fallback);
        for(int i = 0; tiers[i] != eBuildingType::none; i++) {
            const auto b = closestOfType(board, target, tiers[i],
                                         fromX, fromY);
            if(b) return b;
        }
    }
    return nullptr;
}

bool nearestDefender(GameBoard& board, const eCityId target,
                     const eTeamId invaderTid,
                     const int fromX, const int fromY, const int maxDist,
                     int& nX, int& nY) {
    const auto city = board.boardCityWithId(target);
    if(!city) return false;
    int x;
    int y;
    // nearestEnemySoldier returns the nearest soldier hostile to invaderTid that
    // stands on the target city's tiles — i.e. the city's defenders.
    const bool r = city->nearestEnemySoldier(invaderTid, fromX, fromY, x, y);
    if(!r) return false;
    const int dx = fromX - x;
    const int dy = fromY - y;
    if(dx*dx + dy*dy > maxDist*maxDist) return false;
    nX = x;
    nY = y;
    return true;
}

bool invadersStrongerThanDefenders(
        GameBoard& board, const eCityId target,
        const std::vector<SoldierBanner*>& invaders) {
    int invaderCount = 0;
    for(const auto b : invaders) {
        if(!b) continue;
        invaderCount += b->count();
    }
    const eBannerType defTypes[] = {
        eBannerType::hoplite, eBannerType::horseman,
        eBannerType::rockThrower, eBannerType::amazon,
        eBannerType::aresWarrior
    };
    int defenderCount = 0;
    for(const auto bt : defTypes) {
        defenderCount += board.countSoldiers(bt, target);
    }
    // Augustus treats "clearly stronger" as enough of an edge to ignore the
    // garrison; require double so a near-even fight still draws invaders onto
    // the defenders.
    return invaderCount > 2*defenderCount;
}

}
