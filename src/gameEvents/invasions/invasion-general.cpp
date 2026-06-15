#include "invasion-general.h"

#include <cmath>
#include <cstdio>

#include "engine/game-board.h"
#include "engine/eknownendpathfinder.h"
#include "engine/etile.h"
#include "numbers.h"
#include "buildings/ebuilding.h"
#include "characters/soldier-banner.h"
#include "characters/formation-facing.h"

namespace {

// Counts a day-length countdown down by one tick. Returns true while the timer
// is still running (caller should hold), false once it hits zero.
bool drainWait(int& counter, const int by) {
    if(counter <= 0) return false;
    counter -= by;
    if(counter > 0) return true;
    counter = 0;
    return false;
}

const int defendHoldMs = 3000;

eTile* bannerCenterTile(GameBoard& board,
                        const std::vector<SoldierBanner*>& banners) {
    int cx = 0;
    int cy = 0;
    int n = 0;
    eTile* first = nullptr;
    for(const auto b : banners) {
        if(!b || !b->tile()) continue;
        const auto t = b->tile();
        if(!first) first = t;
        cx += t->x();
        cy += t->y();
        n++;
    }
    if(n <= 0) return nullptr;
    cx /= n;
    cy /= n;
    const auto center = board.tile(cx, cy);
    return center ? center : first;
}

}

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
    for(const auto b : banners) {
        if(!b) continue;
        const int c = b->count();
        if(c <= 0) continue;
        ss += c;
    }
    if(ss == 0) return false; // handler handles a wiped force


    // Defend is a latched FSM state. Once inside, ignore fresh hit pings; the
    // state exits only after the hold timer drains and nearby enemies clear.
    if(s.fPhase == eGeneralPhase::defend) {
        for(const auto b : banners) {
            if(b) b->clearLastAttackTile();
        }
        if(drainWait(s.fDefendHold, by)) {
            return false;
        }
        // Pass `by` (not 0) so the cache is allowed to refresh here; passing 0
        // never decrements the countdown so a stale true-cache never clears,
        // causing permanent defend lock when soldiers aren't ticking the banner.
        bool anyEnemy = false;
        for(const auto b : banners) {
            if(b && b->enemyNear(by)) { anyEnemy = true; break; }
        }
        if(anyEnemy) {
            // Hard cap: if enemies are still near 10s after hold drained, force
            // exit anyway to avoid permanent lock when the cache is stale.
            s.fDefendEnemyWait += by;
            if(s.fDefendEnemyWait < 10000) {
                return false;
            }
        }
        s.fDefendEnemyWait = 0;
        s.fPhase = s.fPhaseBeforeDefend;
        s.fWait = 0;
        return false;
    }

    // Enter defense from any campaign phase when a real defender hits.
    eTile* attackTile = nullptr;
    for(const auto b : banners) {
        if(b && b->lastAttackTile()) {
            attackTile = b->lastAttackTile();
            break;
        }
    }
    for(const auto b : banners) {
        if(b) b->clearLastAttackTile();
    }
    if(attackTile) {
        s.fPhaseBeforeDefend = s.fPhase;
        s.fPhase = eGeneralPhase::defend;
        s.fDefendHold = defendHoldMs;
        s.fDefendEnemyWait = 0;
        s.fMoveWait = 0;
        const auto fromTile = bannerCenterTile(mBoard, banners);
        s.fMoveFrom = fromTile;
        s.fMoveTo = attackTile;
        s.fCurrentTile = fromTile;
        for(const auto b : banners) {
            if(b) b->cancelSoldierActions();
        }
        if(fromTile) {
            s.fCurrentTile = attackTile;
            int facing, lineDX, lineDY;
            eFormationFacing::facingAndLineToward(
                attackTile->x() - fromTile->x(),
                attackTile->y() - fromTile->y(),
                facing, lineDX, lineDY);
            SoldierBanner::sPlaceFacing(
                const_cast<std::vector<SoldierBanner*>&>(banners),
                attackTile->x(), attackTile->y(),
                mBoard, facing, lineDX, lineDY, 3, 3);
        }
        return false;
    }

    // 14-day pre-invade wait during the initial spread phase.
    if(s.fPhase == eGeneralPhase::spread && drainWait(s.fSpawnWait, by)) {
        return false;
    }

    // 7-day pause after the half-step waypoint move: units stand on the mid spot
    // a full week before the march order goes out. Only the half-step (a
    // repositioning move) sets this; the formation move + pin are direct target
    // actions and are not gated. Drained above the cycle gate so the full tick
    // is subtracted every frame, not only on gate-pass ticks (else it would take
    // far longer than the intended week to elapse).
    if(s.fPhase == eGeneralPhase::march && drainWait(s.fMoveWait, by)) {
        return false;
    }

    // Drop a stale target (building destroyed) so the phase logic below sees
    // the kill. Do NOT repick here: invade owns the repick + march-on order,
    // and silently swapping in a fresh target here would make invade always
    // see a valid target and hold forever, so no new order is ever issued.
    if(!generalTargetValid(s)) s.fTargetTile = nullptr;

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
        const auto target = ensureTarget(s, landingTile);
        if(target) {
            // Half-step from where the formation actually is, so a re-spread
            // after invade does not yank units back to the landing tile. This is
            // a repositioning move: pause 7 days once units arrive.
            const auto from = s.fCurrentTile ? s.fCurrentTile : landingTile;
            const auto halfTile = moveHalfwayToTarget(s, from, target, banners);
            if(halfTile) {
                s.fMoveWait = stallMaxWait;
            }
        } else {
            printf("[invasion-general] spread->march: no target found, stuck\n");
        }
    } break;
    case eGeneralPhase::march: {
        s.fPhase = eGeneralPhase::invade;
        s.fRepinWait = stallMaxWait;
        const auto target = ensureTarget(s, landingTile);
        if(target) {
            const auto from = s.fCurrentTile ? s.fCurrentTile : landingTile;
            // Two orders back to back, no wait: formation move all banners up to
            // the building, then park one banner ON the building tile. Slots land
            // adjacent (sDefaultWalkable rejects building tiles); soldiers path
            // toward those slots, hit the wall, obstacle handler fires, and the
            // clearObstacle combat assignment directs them to attack the building.
            moveToTarget(s, from, target, banners);
            pinOnTarget(s, banners);
        } else {
            printf("[invasion-general] march->invade: no target found, stuck\n");
        }
    } break;
    case eGeneralPhase::invade: {
        if(generalTargetValid(s)) {
            // Target still stands. Re-pin every stallMaxWait days: teleport
            // soldiers to their slots then re-issue the pin so they re-walk
            // toward the building and get clearObstacle assignments.
            s.fRepinWait -= by;
            if(s.fRepinWait <= 0) {
                s.fRepinWait = stallMaxWait;
                pinOnTarget(s, banners);
            }
        } else {
            // Building fell: route back through march to re-run the formation
            // move + pin on the next objective (no half-step, so no wait). Pick
            // the next target now so a null means the campaign is over.
            const auto cur = s.fCurrentTile ? s.fCurrentTile : landingTile;
            const auto target = ensureTarget(s, landingTile);
            if(!target) {
                // No valid targets remain: campaign complete.
                s.fPhase = eGeneralPhase::done;
                return true;
            }
            moveToTarget(s, cur, target, banners);
            pinOnTarget(s, banners);
        }
    } break;
    case eGeneralPhase::done:
        return true;
    case eGeneralPhase::defend:
        return false;
    }
    return false;
}

eTile* InvasionGeneral::ensureTarget(
        eGeneralState& s, eTile* const landingTile) const {
    if(s.fTargetTile) return s.fTargetTile;
    const auto cur = s.fCurrentTile ? s.fCurrentTile : landingTile;
    if(!cur) return nullptr;
    s.fTargetTile = chooseTargetTile(cur->x(), cur->y());
    return s.fTargetTile;
}

eTile* InvasionGeneral::chooseTargetTile(
        const int fromX, const int fromY) const {
    const auto b = InvasionTargeting::pickPriorityTarget(
                mBoard, mTargetCity, mAttackType, fromX, fromY);
    return b ? b->centerTile() : nullptr;
}

eTile* InvasionGeneral::moveHalfwayToTarget(
        eGeneralState& s,
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
    const auto debugFrom = bannerCenterTile(mBoard, banners);
    s.fMoveFrom = debugFrom ? debugFrom : from;
    s.fMoveTo = halfTile;
    s.fCurrentTile = halfTile;
    SoldierBanner::sPlaceFacing(banners, halfTile->x(), halfTile->y(), mBoard,
                                facing, lineDX, lineDY, 3, 3);
    return halfTile;
}

void InvasionGeneral::moveToTarget(
        eGeneralState& s,
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
    const auto debugFrom = bannerCenterTile(mBoard, banners);
    s.fMoveFrom = debugFrom ? debugFrom : from;
    s.fMoveTo = target;
    s.fCurrentTile = target;
    SoldierBanner::sPlaceFacing(banners, target->x(), target->y(), mBoard,
                                facing, lineDX, lineDY, 3, 3);
}

bool InvasionGeneral::pinOnTarget(
        eGeneralState& s,
        const std::vector<SoldierBanner*>& banners) const {
    const auto target = s.fTargetTile;
    if(!target) return false;
    // Pick by role priority (cavalry > melee > missile), closest within each tier.
    auto rolePriority = [](eBannerFormationRole r) {
        switch(r) {
        case eBannerFormationRole::cavalry: return 0;
        case eBannerFormationRole::melee:   return 1;
        case eBannerFormationRole::missile: return 2;
        default:                            return 3;
        }
    };
    SoldierBanner* best = nullptr;
    int bestPri = -1;
    int bestD = -1;
    for(const auto b : banners) {
        if(!b || b->count() <= 0) continue;
        const auto t = b->tile();
        if(!t) continue;
        const int dx = t->x() - target->x();
        const int dy = t->y() - target->y();
        const int d = dx*dx + dy*dy;
        const int pri = rolePriority(b->formationRole());
        if(best == nullptr || pri < bestPri || (pri == bestPri && d < bestD)) {
            best = b; bestPri = pri; bestD = d;
        }
    }
    if(!best) return false;
    // Park ON the building tile. updatePlaces uses sDefaultWalkable so formation
    // slots land outside the building; soldiers path toward those adjacent slots,
    // hit the wall, the obstacle handler fires setCombatBlockage, and the
    // clearObstacle combat assignment directs them to attack the building.
    best->moveTo(target->x(), target->y());
    return true;
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
