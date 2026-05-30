#include "invasion-general.h"

#include <cmath>

#include "engine/game-board.h"
#include "engine/eknownendpathfinder.h"
#include "engine/etile.h"
#include "enumbers.h"
#include "buildings/ebuilding.h"
#include "characters/soldier-banner.h"
#include "characters/formation-facing.h"

namespace {

// Counts a day-length countdown down by one tick. Returns true while the timer
// is still running (caller should hold), false once it hits zero. Shared by the
// 14-day spawn wait and the 7-day half-step move wait.
bool drainWait(int& counter, const int by) {
    if(counter <= 0) return false;
    counter -= by;
    if(counter > 0) return true;
    counter = 0;
    return false;
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

    // Banners that took fire chase their attacker first — checked before any
    // wait drain so attacks during spawn-wait or midpoint-wait are not ignored.
    if(attackEnemiesNear(banners, by)) {
        if(!s.fDefending) {
            printf("[general] DEFEND start phase=%d\n", (int)s.fPhase);
            s.fDefending = true;
            s.fDefendHold = 8000;
            for(const auto b : banners) { if(b) b->setDefending(true); }
            issueDefensiveOrders(banners);
        } else if(s.fDefendHold > 0) {
            s.fDefendHold -= by;
            if(s.fDefendHold < 0) s.fDefendHold = 0;
        }
        return false;
    }
    if(s.fDefending) {
        if(s.fDefendHold > 0) {
            s.fDefendHold -= by;
            if(s.fDefendHold < 0) s.fDefendHold = 0;
            return false; // hold minimum duration before exiting
        }
        printf("[general] DEFEND end -> resume phase=%d\n", (int)s.fPhase);
        s.fDefending = false;
        for(const auto b : banners) { if(b) b->setDefending(false); }
        s.fWait = 0;
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
            const auto halfTile = moveHalfwayToTarget(from, target, banners);
            if(halfTile) {
                s.fMoveFrom = from;
                s.fMoveTo = halfTile;
                s.fCurrentTile = halfTile;
                s.fMoveWait = 7*eNumbers::sDayLength;
            }
        }
    } break;
    case eGeneralPhase::march: {
        s.fPhase = eGeneralPhase::invade;
        const auto target = ensureTarget(s, landingTile);
        if(target) {
            const auto from = s.fCurrentTile ? s.fCurrentTile : landingTile;
            // Two orders back to back, no wait: formation move all banners up to
            // the building, then park one banner ON the building tile. Slots land
            // adjacent (sDefaultWalkable rejects building tiles); soldiers path
            // toward those slots, hit the wall, obstacle handler fires, and the
            // clearObstacle combat assignment directs them to attack the building.
            moveToTarget(from, target, banners);
            pinOnTarget(s, banners);
            s.fMoveFrom = from;
            s.fMoveTo = target;
            s.fCurrentTile = target;
        }
    } break;
    case eGeneralPhase::invade: {
        if(generalTargetValid(s)) {
            // Target still stands: hold. The formation move + pin already landed;
            // re-issuing every cycle re-homes soldiers and jiggles the formation.
            // Local defenders are handled by banner combat brains.
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
            moveToTarget(cur, target, banners);
            pinOnTarget(s, banners);
            s.fMoveFrom = cur;
            s.fMoveTo = target;
            s.fCurrentTile = target;
        }
    } break;
    case eGeneralPhase::done:
        return true;
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

bool InvasionGeneral::pinOnTarget(
        eGeneralState& s,
        const std::vector<SoldierBanner*>& banners) const {
    const auto target = s.fTargetTile;
    if(!target) return false;
    // Pick the banner already closest to the building so the parked unit travels
    // the least and the formation screen stays mostly intact.
    SoldierBanner* best = nullptr;
    int bestD = -1;
    for(const auto b : banners) {
        if(!b || b->count() <= 0) continue;
        const auto t = b->tile();
        if(!t) continue;
        const int dx = t->x() - target->x();
        const int dy = t->y() - target->y();
        const int d = dx*dx + dy*dy;
        if(bestD < 0 || d < bestD) { bestD = d; best = b; }
    }
    if(!best) return false;
    // Park ON the building tile. updatePlaces uses sDefaultWalkable so formation
    // slots land outside the building; soldiers path toward those adjacent slots,
    // hit the wall, the obstacle handler fires setCombatBlockage, and the
    // clearObstacle combat assignment directs them to attack the building.
    best->moveTo(target->x(), target->y());
    return true;
}

bool InvasionGeneral::attackEnemiesNear(
        const std::vector<SoldierBanner*>& banners,
        const int by) const {
    (void)by;
    // Find any banner under fire and get its retaliation point.
    int rx = 0, ry = 0;
    bool helpNeeded = false;
    for(const auto b : banners) {
        if(!b) continue;
        if(b->needsHelp() && b->retaliationPoint(rx, ry)) {
            helpNeeded = true;
            break;
        }
    }
    return helpNeeded;
}

void InvasionGeneral::issueDefensiveOrders(
        const std::vector<SoldierBanner*>& banners) const {
    // Find contact point from any banner under fire.
    int rx = 0, ry = 0;
    bool hasPoint = false;
    for(const auto b : banners) {
        if(!b) continue;
        if(b->needsHelp() && b->retaliationPoint(rx, ry)) {
            hasPoint = true;
            break;
        }
    }
    if(!hasPoint) return;
    printf("[general] DEFENSIVE orders: rally all to %d,%d\n", rx, ry);

    // All banners get formation orders. Hit banners also clear stale strategic
    // blockages (accumulated during march/invade) so soldiers don't keep attacking
    // buildings when the general has switched them to defensive pursuit.
    // Combat slot assignments refill via updateRetaliation on the next tick.
    std::vector<SoldierBanner*> meleeRally;
    for(const auto b : banners) {
        if(!b || b->count() <= 0) continue;
        if(b->formationRole() == eBannerFormationRole::missile) {
            if(b->fightingRealEnemy()) continue; // engaging banner soldier, leave alone
            b->signalRetaliationTarget(rx, ry);
            // Move to a firing position: stop ~range tiles short of the threat.
            const int firingRange = b->soldierRange();
            const auto bt = b->tile();
            int tx = rx, ty = ry;
            if(bt && firingRange > 0) {
                const int dx = rx - bt->x();
                const int dy = ry - bt->y();
                const float dist = std::sqrt((float)(dx*dx + dy*dy));
                if(dist > firingRange) {
                    const float t = (dist - firingRange) / dist;
                    tx = bt->x() + (int)(dx * t);
                    ty = bt->y() + (int)(dy * t);
                }
            }
            const int bx = bt ? bt->x() : -1, by2 = bt ? bt->y() : -1;
            printf("[general]   missile '%s' banner@%d,%d -> firing pos %d,%d (threat %d,%d) range=%d\n",
                   b->name().c_str(), bx, by2, tx, ty, rx, ry, firingRange);
            b->cancelSoldiersAttack();
            b->moveTo(tx, ty);
            b->attackEnemyNearRetaliationPoint();
        } else {
            if(b->fighting()) continue; // melee engaged, never interrupt
            printf("[general]   melee '%s' -> rally\n", b->name().c_str());
            b->cancelSoldiersAttack();
            if(b->needsHelp()) b->clearCombatBlockages();
            meleeRally.push_back(b);
        }
    }

    if(meleeRally.empty()) return;
    // Formation move for melee/cavalry: centroid as "from", contact as target.
    int cx = 0, cy = 0, n = 0;
    for(const auto b : meleeRally) {
        const auto t = b->tile();
        if(t) { cx += t->x(); cy += t->y(); n++; }
    }
    if(n == 0) return;
    cx /= n; cy /= n;
    const auto fromTile = mBoard.tile(cx, cy);
    const auto targetTile = mBoard.tile(rx, ry);
    if(fromTile && targetTile) moveToTarget(fromTile, targetTile, meleeRally);
}

void InvasionGeneral::reissueCurrentOrder(
        eGeneralState& s,
        eTile* const landingTile,
        const std::vector<SoldierBanner*>& banners) const {
    printf("[general] reissue phase=%d\n", (int)s.fPhase);
    switch(s.fPhase) {
    case eGeneralPhase::march: {
        // Banners drifted during retaliation; walk them back to the half-step tile.
        const auto from = s.fMoveFrom ? s.fMoveFrom : landingTile;
        if(s.fCurrentTile) moveToTarget(from, s.fCurrentTile, banners);
    } break;
    case eGeneralPhase::invade: {
        // Resume assault: re-order formation move + pin on the building.
        const auto from = s.fMoveFrom ? s.fMoveFrom : landingTile;
        if(s.fTargetTile) {
            moveToTarget(from, s.fTargetTile, banners);
            pinOnTarget(s, banners);
        }
    } break;
    default:
        break;
    }
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
