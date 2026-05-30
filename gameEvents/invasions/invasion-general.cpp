#include "invasion-general.h"

#include "engine/game-board.h"
#include "engine/eknownendpathfinder.h"
#include "engine/etile.h"
#include "buildings/ebuilding.h"
#include "characters/soldier-banner.h"
#include "characters/formation-facing.h"

InvasionGeneral::InvasionGeneral(GameBoard& board,
                                 const eCityId targetCity,
                                 const eCityId invadingCity,
                                 const InvasionAttackType attackType) :
    mBoard(board),
    mTargetCity(targetCity),
    mInvadingCity(invadingCity),
    mAttackType(attackType) {}

bool InvasionGeneral::advance(eGeneralState& s,
                              eTile* const landingTile,
                              const std::vector<SoldierBanner*>& banners,
                              const int by) const {
    // Count formation strength this tick. ss==0 means the force is wiped; the
    // handler decides defeat (it owns immortals/conquest reporting), so just
    // hold here and let the handler see ss==0 on its own banner scan.
    int ss = 0;
    int stationary = 0;
    int fighting = 0;
    for(const auto b : banners) {
        if(!b) continue;
        const int c = b->count();
        if(c <= 0) continue;
        ss += c;
        if(b->stationary()) stationary += c;
        if(b->fighting()) fighting += c;
    }
    if(ss == 0) return false; // handler handles a wiped force

    // 14-day pre-invade wait during the initial spread phase.
    if(s.fPhase == eGeneralPhase::spread && s.fSpawnWait > 0) {
        s.fSpawnWait -= by;
        if(s.fSpawnWait > 0) return false;
        s.fSpawnWait = 0;
    }

    // Banners that took fire chase their attacker first.
    if(attackEnemiesNear(banners)) return false;

    // Hold until the formation has gathered before the first march. Gate on the
    // spread phase itself: target selection happens after this, so the gate is
    // not bypassed the instant a priority building exists.
    const bool waitingForInitialFormation =
            s.fPhase == eGeneralPhase::spread && fighting == 0;
    if(waitingForInitialFormation && stationary < 0.8*ss) return false;

    // Drop a stale target (building destroyed) before (re)selecting.
    if(!generalTargetValid(s)) s.fTargetTile = nullptr;
    if(!s.fTargetTile) {
        const auto cur = s.fCurrentTile ? s.fCurrentTile : landingTile;
        if(cur) {
            s.fTargetTile = chooseTargetTile(cur->x(), cur->y());
        }
    }

    // Cycle gate: no throttle on the very first spread step, 3000ms after.
    const int wait = s.fPhase == eGeneralPhase::spread ? 0 : 3000;
    s.fWait += by;
    if(s.fWait < wait) {
        if(s.fPhase == eGeneralPhase::spread) s.fPhase = eGeneralPhase::wait;
        return false;
    }
    if(wait > 0) s.fWait -= wait; else s.fWait = 0;

    switch(s.fPhase) {
    case eGeneralPhase::spread:
    case eGeneralPhase::wait: {
        s.fPhase = eGeneralPhase::march;
        if(!s.fTargetTile && landingTile) {
            s.fTargetTile = chooseTargetTile(
                        landingTile->x(), landingTile->y());
        }
        const auto target = s.fTargetTile;
        if(target) {
            // Half-step from where the formation actually is, so a re-spread
            // after invade does not yank units back to the landing tile.
            const auto from = s.fCurrentTile ? s.fCurrentTile : landingTile;
            const auto halfTile = moveHalfwayToTarget(from, target, banners);
            if(halfTile) s.fCurrentTile = halfTile;
        }
    } break;
    case eGeneralPhase::march: {
        s.fPhase = eGeneralPhase::invade;
        const auto target = s.fTargetTile;
        if(target) {
            const auto from = s.fCurrentTile ? s.fCurrentTile : landingTile;
            moveToTarget(from, target, banners);
            s.fCurrentTile = target;
        }
    } break;
    case eGeneralPhase::invade: {
        if(generalTargetValid(s)) {
            // Target still stands: hold the formation on it and keep hammering
            // until destroyed. No new orders until the goal is achieved.
            const auto target = s.fTargetTile;
            const auto from = s.fCurrentTile ? s.fCurrentTile : landingTile;
            moveToTarget(from, target, banners);
        } else {
            // Building fell: pick the next objective and march on it.
            const auto cur = s.fCurrentTile ? s.fCurrentTile : landingTile;
            s.fTargetTile = cur ? chooseTargetTile(
                        cur->x(), cur->y()) : nullptr;
            if(!s.fTargetTile) {
                // No valid targets remain: campaign complete.
                s.fPhase = eGeneralPhase::done;
                return true;
            }
            s.fPhase = eGeneralPhase::march;
        }
    } break;
    case eGeneralPhase::done:
        return true;
    }
    return false;
}

eTile* InvasionGeneral::chooseTargetTile(
        const int fromX, const int fromY) const {
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
    int facing;
    int lineDX;
    int lineDY;
    eFormationFacing::facingAndLineToward(target->x() - halfTile->x(),
                                          target->y() - halfTile->y(),
                                          facing, lineDX, lineDY);
    SoldierBanner::sPlaceFacing(banners, halfTile->x(), halfTile->y(), mBoard,
                                facing, lineDX, lineDY, 3, 3);
    return halfTile;
}

void InvasionGeneral::moveToTarget(
        eTile* const from,
        eTile* const target,
        const std::vector<SoldierBanner*>& banners) const {
    if(!target) return;
    // Spread banners into a formation facing the target instead of stacking
    // every banner on the single target tile. Facing is derived from the
    // approach vector (from -> target) so the line points at the objective.
    const int anchorX = from ? from->x() : target->x();
    const int anchorY = from ? from->y() : target->y();
    int facing;
    int lineDX;
    int lineDY;
    eFormationFacing::facingAndLineToward(target->x() - anchorX,
                                          target->y() - anchorY,
                                          facing, lineDX, lineDY);
    SoldierBanner::sPlaceFacing(banners, target->x(), target->y(), mBoard,
                                facing, lineDX, lineDY, 3, 3);
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

bool InvasionGeneral::generalTargetValid(const eGeneralState& s) const {
    const auto t = s.fTargetTile;
    if(!t) return false;
    if(t->cityId() != mTargetCity) return false;
    const auto b = t->underBuilding();
    if(!b) return false;
    if(b->cityId() != mTargetCity) return false;
    if(!b->enabled()) return false;
    return eBuilding::sAttackable(b->type());
}
