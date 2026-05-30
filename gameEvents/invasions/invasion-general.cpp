#include "invasion-general.h"

#include "engine/game-board.h"
#include "engine/eknownendpathfinder.h"
#include "engine/etile.h"
#include "characters/soldier-banner.h"

InvasionGeneral::InvasionGeneral(GameBoard& board,
                                 const eCityId targetCity,
                                 const eCityId invadingCity,
                                 const InvasionAttackType attackType) :
    mBoard(board),
    mTargetCity(targetCity),
    mInvadingCity(invadingCity),
    mAttackType(attackType) {}

eTile* InvasionGeneral::chooseTargetTile(
        const int fromX, const int fromY,
        const std::vector<SoldierBanner*>& banners) const {
    (void)banners;

    // The general owns strategy, so it picks buildings. Nearby defenders are
    // handled by banner/unit combat code and should not pull the whole invasion
    // off its objective every few seconds.
    const auto b = InvasionTargeting::pickPriorityTarget(
                mBoard, mTargetCity, mAttackType, fromX, fromY);
    if(b) return b->centerTile();
    return nullptr;
}

eTile* InvasionGeneral::moveHalfwayToTarget(
        eTile* const from,
        eTile* const target,
        const std::vector<SoldierBanner*>& banners) const {
    if(!from || !target) return nullptr;
    eKnownEndPathFinder pf([](eTileBase* const t) {
        return t->walkableTerrain();
    }, target);
    const int w = mBoard.width();
    const int h = mBoard.height();
    const bool r = pf.findPath({0, 0, w, h}, from, 1000, false, w, h);
    if(!r) return nullptr;
    std::vector<eTile*> path;
    pf.extractPath(path, mBoard);
    if(path.empty()) return nullptr;
    const auto halfTile = path[path.size()/2];
    SoldierBanner::sPlaceFacing(banners, halfTile->x(), halfTile->y(), mBoard,
                                180, 1, 0, 3, 3);
    return halfTile;
}

void InvasionGeneral::moveToTarget(
        eTile* const target,
        const std::vector<SoldierBanner*>& banners) const {
    if(!target) return;
    for(const auto b : banners) {
        if(!b) continue;
        b->moveTo(target->x(), target->y());
    }
}

bool InvasionGeneral::attackEnemiesNear(
        const std::vector<SoldierBanner*>& banners) const {
    bool helpNeeded = false;
    for(const auto b : banners) {
        if(!b) continue;
        if(b->needsHelp()) {
            helpNeeded = true;
            break;
        }
    }
    if(!helpNeeded) return false;

    bool ordered = false;
    for(const auto b : banners) {
        if(!b) continue;
        if(b->attackEnemyNearRetaliationPoint()) ordered = true;
    }
    return ordered;
}
