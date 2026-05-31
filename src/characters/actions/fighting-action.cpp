#include "fighting-action.h"

#include <math.h>
#include <cstdio>
#include <algorithm>
#include "erand.h"


#include "characters/actions/ewaitaction.h"
#include "characters/efightingcharacter.h"
#include "emovetoaction.h"
#include "buildings/ebuilding.h"
#include "enumbers.h"
#include "audio/sounds.h"
#include "fileIO/esavearchive.h"
#include "vec2.h"

#include "missiles/erockmissile.h"
#include "missiles/earrowmissile.h"
#include "missiles/espearmissile.h"
#include "combat-timing.h"
#include "characters/soldier-banner.h"

namespace {
bool sCanAttackCharacter(const eCharacter* const c) {
    if(c->isSoldier()) return true;
    const auto type = c->type();
    return type == eCharacterType::wolf ||
           type == eCharacterType::enemyBoat ||
           type == eCharacterType::trireme ||
           c->isImmortal();
}

bool sCanStandOn(eTile* const t) {
    if(!t) return false;
    const auto ubt = t->underBuildingType();
    if(ubt == eBuildingType::none) return true;
    return eBuilding::sWalkableBuilding(ubt);
}

int sBulldozeTileCost(eTileBase* const tile) {
    const auto ubt = tile->underBuildingType();
    if(ubt == eBuildingType::none) return 1;
    if(eBuilding::sWalkableBuilding(ubt)) return 1;
    return 500;
}

// Best firing tile for a ranged unit: a tile within `spread` of its own anchor
// (formation slot) from which the enemy at (ex,ey) sits within `range`. Returns
// the candidate closest to the anchor, or null if none brings the enemy into
// range. Never returns the enemy tile — the unit holds the line and shoots.
eTile* sFindFiringTile(FightingAction* const act,
                       eTile* const anchor,
                       const int ex, const int ey,
                       const int range, const int spread) {
    if(!anchor) return nullptr;
    auto& brd = act->character()->getBoard();
    const int ax = anchor->x();
    const int ay = anchor->y();
    eTile* best = nullptr;
    int bestAnchorDist = 0;
    for(int i = -spread; i <= spread; i++) {
        for(int j = -spread; j <= spread; j++) {
            const int cx = ax + i;
            const int cy = ay + j;
            const int edx = ex - cx;
            const int edy = ey - cy;
            // Enemy must be within firing range of the candidate, but not so
            // close it forces melee — keep at least 1 tile of standoff.
            const int enemyDist = std::max(abs(edx), abs(edy));
            if(enemyDist > range || enemyDist < 1) continue;
            const auto t = brd.tile(cx, cy);
            if(!sCanStandOn(t)) continue;
            const int anchorDist = abs(i) + abs(j);
            if(!best || anchorDist < bestAnchorDist) {
                best = t;
                bestAnchorDist = anchorDist;
            }
        }
    }
    return best;
}

// Hard cap of attackers that may LOCK onto one enemy at attack time. Augustus
// uses 2 (combat.c: `num_attackers >= 2` ⇒ skip). A would-be 3rd attacker looks
// elsewhere this tick. Enforced where the attack target is set, not at walk time.
const int sMaxAttackersPerEnemy = 2;

// Nearest attackable enemy to `c` within a square of `range`. Two-pass like
// Augustus figure_combat_get_target_for_enemy: prefer the nearest enemy that no
// one is at the cap on yet (free), and only if none is free fall back to the
// nearest of any. NO soft distance penalty during the approach — penalizing a
// close foe made a soldier walk past the man beside him toward a distant free
// one, then re-pick mid-walk when that one got claimed, dithering between them.
// The 2-cap (checked here via `free`) does the spreading; raw distance keeps
// each soldier committed to whoever is actually closest. Buildings ignored.
eCharacter* sNearestEnemy(eCharacter* const c, const int range) {
    const auto ct = c->tile();
    if(!ct) return nullptr;
    const int tx = ct->x();
    const int ty = ct->y();
    const auto tid = c->teamId();
    auto& brd = c->getBoard();
    eCharacter* nearestFree = nullptr;
    int nearestFreeDist = 0;
    eCharacter* nearestAny = nullptr;
    int nearestAnyDist = 0;
    for(int i = -range; i <= range; i++) {
        for(int j = -range; j <= range; j++) {
            const auto t = brd.tile(tx + i, ty + j);
            if(!t) continue;
            for(const auto& cc : t->characters()) {
                if(cc->dead()) continue;
                if(!eTeamIdHelpers::isEnemy(cc->teamId(), tid)) continue;
                if(!sCanAttackCharacter(cc.get())) continue;
                const int dist = std::max(abs(i), abs(j));
                if(!nearestAny || dist < nearestAnyDist) {
                    nearestAny = cc.get();
                    nearestAnyDist = dist;
                }
                if(cc->targetedByCount() < sMaxAttackersPerEnemy) {
                    if(!nearestFree || dist < nearestFreeDist) {
                        nearestFree = cc.get();
                        nearestFreeDist = dist;
                    }
                }
            }
        }
    }
    return nearestFree ? nearestFree : nearestAny;
}
}

AttackTarget::AttackTarget() :
    mC(nullptr), mB(nullptr) {}

AttackTarget::AttackTarget(eCharacter* const c) :
    mC(c), mB(nullptr) {}

AttackTarget::AttackTarget(eBuilding* const b) :
    mC(nullptr), mB(b) {}

eTile* AttackTarget::tile() const {
    if(mC) return mC->tile();
    if(mB) return mB->centerTile();
    return nullptr;
}

bool AttackTarget::valid() const {
    return mC || mB;
}

bool AttackTarget::takeDamage(const double a, eCharacter* const attacker) {
    if(mC) return mC->takeDamage(a, attacker);
    if(mB) return mB->takeDamage(a);
    return true;
}

bool AttackTarget::takeMeleeDamage(const double a, eCharacter* const attacker) {
    if(mC) return mC->takeMeleeDamage(a, attacker);
    if(mB) return mB->takeDamage(a);
    return true;
}

bool AttackTarget::dead() const {
    if(mC) return mC->dead();
    if(mB) return false;
    return true;
}

void AttackTarget::clear() {
    mC = nullptr;
    mB = nullptr;
}

int AttackTarget::armor() const {
    if(mC) return mC->armor();
    return 0;
}

int AttackTarget::armorVsMissiles() const {
    if(mC) return mC->armorVsMissiles();
    return 0;
}

bool AttackTarget::building() const {
    return mB;
}

eBuilding* AttackTarget::buildingPtr() const {
    return mB.get();
}

double AttackTarget::absX() const {
    if(mC) return mC->absX();
    if(mB) {
        const auto t = mB->centerTile();
        if(!t) return 0.;
        return t->x();
    }
    return 0.;
}

double AttackTarget::absY() const {
    if(mC) return mC->absY();
    if(mB) {
        const auto t = mB->centerTile();
        if(!t) return 0.;
        return t->y();
    }
    return 0.;
}

void AttackTarget::serialize(eSaveArchive& ar, GameBoard& board) {
    ar.characterField("character", &board, mC);
    ar.buildingField("building", &board, mB);
}

FightingAction::~FightingAction() {
    releaseClaim();
}

void FightingAction::cancelAttack() {
    mAttack = false;
    mAttackRanged = false;
    mAttackTarget.clear();
    mAttackTime = 0;
    mMeleeTime = 0;
    mMissile = 0;
    releaseClaim();
    const auto c = character();
    c->setPlayFightSound(false);
    c->setActionType(mSavedAction);
}

void FightingAction::clearSavedMovement() {
    mSavedMove = FightingSavedMove::none;
    mSavedMoveX = 0;
    mSavedMoveY = 0;
    mSavedMoveDistance = 0;
    mWaitGoHomeRemaining = 0;
}

void FightingAction::releaseClaim() {
    if(!mClaimedTarget) return;
    mClaimedTarget->decTargetedBy();
    mClaimedTarget = nullptr;
}

void FightingAction::claimTarget(eCharacter* const c) {
    releaseClaim();
    if(!c) return;
    c->incTargetedBy();
    mClaimedTarget = c;
}

void FightingAction::sSignalBeingAttack(
    eCharacter* const attacked,
    eCharacter* const by,
    GameBoard& brd) {
    const auto tt = by->tile();
    if(!tt) return;
    const int ttx = tt->x();
    const int tty = tt->y();
    sSignalBeingAttack(attacked, ttx, tty, brd);
}

void FightingAction::sSignalBeingAttack(eCharacter * const attacked,
                                         const int ttx, const int tty,
                                         GameBoard &brd) {
    const auto att = attacked->tile();
    if(!att) return;
    const int attx = att->x();
    const int atty = att->y();
    const auto atid = attacked->teamId();
    const int range = eNumbers::sSoldierBeingAttackedCallRange;
    for(int ii = -range; ii <= range; ii++) {
        for(int jj = -range; jj <= range; jj++) {
            const auto tt = brd.tile(attx + ii, atty + jj);
            if(!tt) continue;
            const auto& ccchars = tt->characters();
            for(const auto& ccc : ccchars) {
                const auto sss = dynamic_cast<eFightingCharacter*>(ccc.get());
                if(!sss) continue;
                if(ccc->teamId() != atid) continue;
                if(ccc->dead()) continue;

                const auto aaa = sss->fightingAction();
                if(aaa) aaa->beingAttacked(ttx, tty);
            }
        }
    }
}

LookForEnemyState FightingAction::lookForEnemy(const int by) {
    if(mSavedMove == FightingSavedMove::waitGoHome) {
        mWaitGoHomeRemaining -= by;
        if(mWaitGoHomeRemaining <= 0) {
            mWaitGoHomeRemaining = 0;
            mSavedMove = FightingSavedMove::none;
            setCurrentAction(nullptr);
            goHome();
        }
        return LookForEnemyState::none;
    }
    const int rangeAttackCheck = 250;
    const int lookForEnemyCheck = 250;

    const auto c = character();
    if(c->dead()) return LookForEnemyState::dead;
    int range = c->range();
    auto& brd = c->getBoard();
    const auto ct = c->tile();
    if(!ct) return LookForEnemyState::dead;
    const int tx = ct->x();
    const int ty = ct->y();
    const auto tid = c->teamId();

    if(mAttack) {
        bool finishAttack = false;
        const bool ranged = mAttackRanged;
        const int missileCheck = c->missileFreq() > 0 ? c->missileFreq() * 10 : 200;
        if(range > 0 && mAttackTarget.valid() && ranged) {
            mMissile += by;
            if(mMissile > missileCheck) {
                mMissile = mMissile - missileCheck;
                const auto tt = mAttackTarget.tile();
                const int ttx = tt ? tt->x() : 0;
                const int tty = tt ? tt->y() : 0;
                const double dx = tx - ttx;
                const double dy = ty - tty;
                const double dist = sqrt(dx*dx + dy*dy);
                const auto ct = c->type();
                if(ct == eCharacterType::amazon ||
                    ct == eCharacterType::centaurArcher ||
                    ct == eCharacterType::egyptianArcher ||
                    ct == eCharacterType::atlanteanArcher ||
                    ct == eCharacterType::phoenicianArcher ||
                    ct == eCharacterType::persianArcher ||
                    ct == eCharacterType::trireme) {
                    eMissile::sCreate<eArrowMissile>(brd, tx, ty, 0.5,
                                                     ttx, tty, 0.5, 0.25*dist);
                } else if(ct == eCharacterType::trojanSpearthrower ||
                          ct == eCharacterType::oceanidSpearthrower) {
                    eMissile::sCreate<eSpearMissile>(brd, tx, ty, 0.5,
                                                     ttx, tty, 0.5, 0.1*dist);
                } else {
                    eMissile::sCreate<eRockMissile>(brd, tx, ty, 0.5,
                                                    ttx, tty, 0.5, 0.5*dist);
                }
                if(!mAttackTarget.dead()) {
                    const double arm = mAttackTarget.armorVsMissiles();
                    const double atk = c->missileAttack() > 0 ? c->missileAttack() : c->attack();
                    const double dmg = atk - arm;
                    const double att = dmg > 0 ? dmg : 0.01;
                    bool d;
                    if(const auto b = mAttackTarget.buildingPtr()) {
                        d = b->takeDamage(att);
                        if(!d && !b->isOnFire() &&
                           b->hp() <= eBuilding::sMaxHp(b->type()) * 0.5 &&
                           eRand::rand() % 2 == 0) {
                            b->setOnFire(true);
                            d = true;
                        }
                    } else {
                        d = mAttackTarget.takeDamage(att, c);
                    }
                    if(d) finishAttack = true;
                }
            }
        }
        mAttackTime += by;
        if(!finishAttack) finishAttack = !mAttackTarget.valid() ||
                                          mAttackTarget.dead() ||
                                          (mAttackTime > 1000 && !mAttackTarget.building());
        if(!ranged && mAttackTarget.valid() && !mAttackTarget.dead()) {
            const double arm = mAttackTarget.armor();
            const double atk = c->attack();
            const double dmg = atk - arm;
            const double per = dmg > 0 ? dmg : 0.;
            mMeleeTime += by;
            const int cycleMs = CombatTiming::meleeCycleMs(*c);
            const int animMs = CombatTiming::meleeAnimationMs(*c);
            const auto wantedAction = mMeleeTime + animMs >= cycleMs ?
                                      eCharacterActionType::fight :
                                      eCharacterActionType::stand;
            if(c->actionType() != wantedAction) c->setActionType(wantedAction);
            if(mMeleeTime >= cycleMs) {
                mMeleeTime -= cycleMs;
                if(c->actionType() != eCharacterActionType::fight) {
                    c->setActionType(eCharacterActionType::fight);
                }
                bool d;
                if(const auto b = mAttackTarget.buildingPtr()) {
                    d = b->takeDamage(per);
                    if(!d && !b->isOnFire() &&
                       b->hp() <= eBuilding::sMaxHp(b->type()) * 0.5 &&
                       eRand::rand() % 2 == 0) {
                        b->setOnFire(true);
                        d = true;
                    }
                } else {
                    d = mAttackTarget.takeMeleeDamage(per, c);
                }
                if(d) finishAttack = true;
            }
        }
        if(finishAttack) {
            mAttack = false;
            mAttackTarget.clear();
            releaseClaim();
            mAttackTime = 0;
            mRangeAttack = rangeAttackCheck;
            c->setActionType(mSavedAction);
            c->setPlayFightSound(false);
            mAttackRanged = false;
            mLookForEnemy = lookForEnemyCheck;
        } else {
            return LookForEnemyState::attacking;
        }
    }
    const vec2d cpos{c->absX(), c->absY()};

    const auto setAttackTarget = [&](const stdsptr<eCharacter>& cc,
                                      const bool range) {
        const vec2d ccpos{cc->absX(), cc->absY()};
        const vec2d posdif = ccpos - cpos;
        mAttackTarget = AttackTarget(cc.get());
        // Count ourselves as an attacker of this foe (Augustus num_attackers),
        // but ONLY for melee. The 2-cap is a melee-adjacency lock; ranged units
        // pepper freely and must not consume a melee slot, else archers would
        // push melee soldiers off a target. Released on finish/cancel/dtor.
        if(!range) claimTarget(cc.get());
        mAttack = true;
        mAttackRanged = range;
        c->setPlayFightSound(true);
        mAttackTime = 0;
        mSavedAction = c->actionType();
        c->setActionType(range ? eCharacterActionType::fight2 :
                              eCharacterActionType::fight);
        eSounds::playAttackSound(c);
        mAngle = posdif.angle();
        const auto o = sAngleOrientation(mAngle);
        c->setOrientation(o);
    };

    stdsptr<eCharacter> secondOption;
    stdsptr<eCharacter> thirdOption;
    stdsptr<eCharacter> cappedAdjacent; // already-full foe, but it's right here
    const int adjacencyCheckMs = 250;
    mAdjacencyCheck += by;
    const bool adjacencyScan = mAdjacencyCheck > adjacencyCheckMs;
    if(adjacencyScan) mAdjacencyCheck -= adjacencyCheckMs;
    // Unit pass first: enemy units always win over buildings, so a soldier
    // turns on whoever is hitting him instead of clubbing an adjacent house.
    if(adjacencyScan) {
        for(int i = -1; i <= 1; i++) {
            for(int j = -1; j <= 1; j++) {
                const auto t = brd.tile(tx + i, ty + j);
                if(!t) continue;
                const auto& chars = t->characters();
                for(const auto& cc : chars) {
                    const auto cctype = cc->type();
                    const auto cctid = cc->teamId();
                    if(!eTeamIdHelpers::isEnemy(cctid, tid)) continue;
                    if(cc->dead()) continue;
                    if(!sCanAttackCharacter(cc.get())) continue;
                    if(!cc->isSoldier() && cctype != eCharacterType::wolf) {
                        if(cc->isImmortal()) {
                            thirdOption = cc;
                        } else if(cctype == eCharacterType::enemyBoat ||
                                   cctype == eCharacterType::trireme) {
                            secondOption = cc;
                        }
                        continue;
                    }
                    if(cc->targetedByCount() >= sMaxAttackersPerEnemy) {
                        if(!cappedAdjacent) cappedAdjacent = cc;
                        continue;
                    }
                    setAttackTarget(cc, false);
                    return LookForEnemyState::attacking;
                }
            }
        }
        if(cappedAdjacent) {
            setAttackTarget(cappedAdjacent, false);
            return LookForEnemyState::attacking;
        }
        if(secondOption) {
            setAttackTarget(secondOption, false);
            return LookForEnemyState::attacking;
        }
        if(thirdOption) {
            setAttackTarget(thirdOption, false);
            return LookForEnemyState::attacking;
        }
    }
    // Building pass only after no enemy unit was attackable this tick. Ranged
    // units skip melee building pass — they shoot from range or hold.
    if(adjacencyScan && range == 0) {
        for(int i = -1; i <= 1; i++) {
            for(int j = -1; j <= 1; j++) {
                const auto t = brd.tile(tx + i, ty + j);
                if(!t) continue;
                const auto ub = t->underBuilding();
                if(ub && ub->isOnFire()) continue;
                const bool r = attackBuilding(t, false);
                if(r) return LookForEnemyState::attacking;
            }
        }
    }

    if(range > 0) {
        mRangeAttack += by;
        if(mRangeAttack > rangeAttackCheck) {
            mRangeAttack -= rangeAttackCheck;
            // Unit pass first: shoot an enemy unit in range before any building.
            for(int i = -range; i <= range; i++) {
                for(int j = -range; j <= range; j++) {
                    const auto t = brd.tile(tx + i, ty + j);
                    if(!t) continue;
                    const auto& chars = t->characters();
                    for(const auto& cc : chars) {
                        const auto cctype = cc->type();
                        const auto cctid = cc->teamId();
                        if(!eTeamIdHelpers::isEnemy(cctid, tid)) continue;
                        if(cc->dead()) continue;
                        if(!sCanAttackCharacter(cc.get())) continue;
                        if(!cc->isSoldier() && cctype != eCharacterType::wolf) {
                            if(cc->isImmortal()) {
                                thirdOption = cc;
                            } else if(cctype == eCharacterType::enemyBoat ||
                                       cctype == eCharacterType::trireme) {
                                secondOption = cc;
                            }
                            continue;
                        }
                        // No attacker cap on ranged: many archers may pepper one
                        // target. The 2-cap is a melee-adjacency lock only
                        // (Augustus num_attackers), not for arrows.
                        setAttackTarget(cc, true);
                        sSignalBeingAttack(cc.get(), c, brd);
                        return LookForEnemyState::attacking;
                    }
                }
            }

            if(secondOption) {
                setAttackTarget(secondOption, true);
                return LookForEnemyState::attacking;
            }
            if(thirdOption) {
                setAttackTarget(thirdOption, true);
                return LookForEnemyState::attacking;
            }

        }
    }

    // Self-positioning. The banner march (SoldierBanner::updateCombat, the
    // Augustus per-formation brain) brings the formation onto the defenders;
    // this lets each idle soldier close the last few tiles onto its OWN nearest
    // free target. Without it only the front two ranks are adjacent and lock —
    // the rest stand in formation soaking hits. Augustus enemy_fighting does the
    // same: every fighting figure that has no opponent walks to its nearest
    // non-targeted legion (soft penalty + 2-cap spread it across the line).
    if(allowsSelfPositioning() && (!currentAction() || mOverwrittableAction)) {
        mLookForEnemy += by;
        if(mLookForEnemy > lookForEnemyCheck) {
            mLookForEnemy -= lookForEnemyCheck;
            const int erange = sRangedDetectRange(range);
            const auto tgt = sNearestEnemy(c, erange);
            if(tgt) {
                const auto tt = tgt->tile();
                if(tt) {
                    const int ttx = tt->x();
                    const int tty = tt->y();
                    const int curDist = std::max(abs(ttx - tx), abs(tty - ty));
                    if(range > 0) {
                        // Ranged: hold if already in firing range, else step to
                        // a firing tile near our slot. Never charge the enemy.
                        if(curDist <= range && curDist >= 1) {
                            return LookForEnemyState::attacking;
                        }
                        const auto fire = sFindFiringTile(
                            this, repositionAnchor(), ttx, tty, range, 4);
                        if(fire) {
                            setOverwrittableAction(false);
                            goTo(fire->x(), fire->y(), 0);
                            return LookForEnemyState::attacking;
                        }
                    } else {
                        // Melee: already adjacent ⇒ the adjacency scan locks on
                        // next tick, hold. Otherwise walk onto the target tile
                        // (dist 1) so we end up adjacent. Picks the nearest free
                        // foe, so surplus soldiers fan out to other defenders
                        // rather than queueing behind the front two.
                        if(curDist <= 1) {
                            return LookForEnemyState::attacking;
                        }
                        // Commit to the walk: mark it non-overwrittable so the
                        // next tick's re-pick and any beingAttacked signal don't
                        // yank us toward a different foe mid-stride. Cleared on
                        // arrival by goTo's finish action, so we re-evaluate only
                        // once we get there. Without this the soldier re-paths
                        // every 500ms and bounces between targets, never landing.
                        setOverwrittableAction(false);
                        goTo(ttx, tty, 1);
                        return LookForEnemyState::attacking;
                    }
                }
            }
            // No enemy unit in range. Melee soldiers close on the nearest enemy
            // building so the WHOLE formation razes it, not just the front rank
            // that happened to arrive adjacent. Without this, back ranks have no
            // unit to chase and no building in their own path, so they stand idle
            // while two soldiers slowly chip the wall — the "takes a month, most
            // stand by" bug. Ranged units skip this; their range building pass
            // above already shoots structures from the line.
            if(!tgt && range == 0) {
                const int berange = sRangedDetectRange(range);
                eBuilding* nb = nullptr;
                int nbx = 0;
                int nby = 0;
                int nbDist = 0;
                for(int i = -berange; i <= berange; i++) {
                    for(int j = -berange; j <= berange; j++) {
                        const auto t = brd.tile(tx + i, ty + j);
                        if(!t) continue;
                        const auto ub = t->underBuilding();
                        if(!ub) continue;
                        if(!eTeamIdHelpers::isEnemy(ub->teamId(), tid)) continue;
                        if(!eBuilding::sAttackable(ub->type())) continue;
                        const int d = std::max(abs(i), abs(j));
                        if(!nb || d < nbDist) {
                            nb = ub;
                            nbx = t->x();
                            nby = t->y();
                            nbDist = d;
                        }
                    }
                }
                if(nb) {
                    if(nbDist <= 1) {
                        // Adjacent: the building pass locks on next scan, hold.
                        return LookForEnemyState::attacking;
                    }
                    // Walk to a free tile beside the building so soldiers ring it
                    // instead of stacking one tile. Same spread as the unit close.
                    eTile* dst = nullptr;
                    int dstDist = 0;
                    for(int ai = -1; ai <= 1; ai++) {
                        for(int aj = -1; aj <= 1; aj++) {
                            if(!ai && !aj) continue;
                            const auto at = brd.tile(nbx + ai, nby + aj);
                            if(!sCanStandOn(at)) continue;
                            bool occupied = false;
                            for(const auto& oc : at->characters()) {
                                if(oc.get() == c) continue;
                                if(oc->dead()) continue;
                                if(oc->isSoldier() || oc->isImmortal() ||
                                   oc->type() == eCharacterType::wolf) {
                                    occupied = true;
                                    break;
                                }
                            }
                            if(occupied) continue;
                            const int d = std::max(abs(at->x() - tx),
                                                   abs(at->y() - ty));
                            if(!dst || d < dstDist) {
                                dst = at;
                                dstDist = d;
                            }
                        }
                    }
                    if(dst) {
                        setOverwrittableAction(false);
                        goTo(dst->x(), dst->y(), 0);
                        return LookForEnemyState::attacking;
                    }
                }
            }
        }
    }

    return LookForEnemyState::none;
}

eTile* FightingAction::repositionAnchor() const {
    return character()->tile();
}

bool FightingAction::attackBuilding(eTile* const t, const bool range) {
    const auto ub = t->underBuilding();
    if(!ub) return false;
    const auto c = character();
    const auto tid = c->teamId();
    if(!eTeamIdHelpers::isEnemy(ub->teamId(), tid)) return false;
    const vec2d cpos{c->absX(), c->absY()};
    const bool att = eBuilding::sAttackable(ub->type());
    if(!att) return false;
    mAttackTarget = AttackTarget(ub);
    mAttack = true;
    mAttackRanged = range;
    c->setPlayFightSound(true);
    mAttackTime = 0;
    mSavedAction = c->actionType();
    c->setActionType(range ? eCharacterActionType::fight2 :
                     eCharacterActionType::fight);
    eSounds::playAttackSound(c);
    const vec2d ccpos{1.*t->x(), 1.*t->y()};
    const vec2d posdif = ccpos - cpos;
    mAngle = posdif.angle();
    const auto o = sAngleOrientation(mAngle);
    c->setOrientation(o);
    return true;
}

void FightingAction::goTo(const int fx, const int fy,
                           const int dist,
                           const eAction& findFailAct,
                           const eAction& findFinishAct) {
    goToInternal(fx, fy, dist, findFailAct, findFinishAct, false);
}

void FightingAction::goToInternal(const int fx, const int fy,
                                   const int dist,
                                   const eAction& findFailAct,
                                   const eAction& findFinishAct,
                                   const bool forceAttacker) {
    const auto c = character();
    const auto t = c->tile();
    const int sx = t->x();
    const int sy = t->y();
    if(abs(fx - sx) <= dist && abs(fy - sy) <= dist) {
        mSavedMove = FightingSavedMove::none;
        return;
    }
    mSavedMove = FightingSavedMove::goTo;
    mSavedMoveX = fx;
    mSavedMoveY = fy;
    mSavedMoveDistance = dist;

    const auto hha = [fx, fy, dist](eThreadTile* const t) {
        return abs(t->x() - fx) <= dist && abs(t->y() - fy) <= dist;
    };

    const auto finishAct = std::make_shared<SA_goToFinish>(
        board(), c);

    auto& board = this->board();
    const auto tid = c->teamId();
    // Attacker pathing (ignore buildings, smash any in the way) keys off enmity
    // to the GROUND being crossed — but check both ends. A unit standing on a
    // neutral path tile next to the besieged city would otherwise get plain
    // walkable and the planner, unable to cross the enemy buildings around the
    // destination, falls back to a degenerate straight line clipping through
    // them. Treat the move as an assault if either the start tile OR the
    // destination tile belongs to an enemy city, matching Augustus enemies
    // (TERRAIN_USAGE_ENEMY): they path through and destroy obstructions.
    const auto startTid = board.cityIdToTeamId(t->cityId());
    bool attackBuildings = eTeamIdHelpers::isEnemy(tid, startTid);
    if(!attackBuildings) {
        const auto dt = board.tile(fx, fy);
        if(dt) {
            const auto destTid = board.cityIdToTeamId(dt->cityId());
            attackBuildings = eTeamIdHelpers::isEnemy(tid, destTid);
        }
    }
    // Invaders route AROUND buildings by default; only fall back to bulldozing
    // (sCreateAttacker + obstacle handler) when no path around exists, which the
    // wrapped find-fail below detects and retries with forceAttacker=true.
    const bool pathAround = attackBuildings && prefersPathAround() &&
                            !forceAttacker;

    stdsptr<eWalkableObject> pathFindWalkable;
    stdsptr<eWalkableObject> moveWalkable;
    if(c->isBoat()) {
        pathFindWalkable =
            eWalkableObject::sCreateDeepWater();
    } else if(attackBuildings && !pathAround) {
        pathFindWalkable = eWalkableObject::sCreateAttacker();
        moveWalkable = eWalkableObject::sCreateDefault();
    } else {
        pathFindWalkable =
            eWalkableObject::sCreateDefault();
    }

    eAction failAct = findFailAct;
    if(pathAround) {
        const stdptr<FightingAction> tptr(this);
        failAct = [tptr, fx, fy, dist, findFailAct, findFinishAct]() {
            if(!tptr) {
                if(findFailAct) findFailAct();
                return;
            }
            // No route around: walled off, smash through to the target.
            tptr->goToInternal(fx, fy, dist, findFailAct, findFinishAct, true);
        };
    }

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setFailAction(finishAct);
    a->setFinishAction(finishAct);
    a->setFindFailAction(failAct);
    if(attackBuildings && !pathAround) {
        a->setObsticleHandler(obsticleHandler());
        a->setTileDistance(sBulldozeTileCost);
    }

    const stdptr<eCharacter> cptr(character());
    const stdptr<FightingAction> tptr(this);
    a->setFoundAction([cptr, tptr, findFinishAct]() {
        if(!cptr) return;
        cptr->setActionType(eCharacterActionType::walk);
        if(!tptr) return;
        if(findFinishAct) findFinishAct();
    });
    a->start(hha, pathFindWalkable, moveWalkable,
             [fx, fy](eThreadBoard& board) {
                 return board.tile(fx, fy);
             });
    setCurrentAction(a);
}

void FightingAction::beingAttacked(eCharacter* const ss) {
    const auto tt = ss->tile();
    const int ttx = tt->x();
    const int tty = tt->y();
    beingAttacked(ttx, tty);
}

void FightingAction::beingAttacked(int ttx, int tty) {
    if(mAttack) return;
    if(!mOverwrittableAction && currentAction()) return;
    const int range = character()->range();
    if(range > 0) {
        // Ranged: the attacker is NOT the destination. If already within firing
        // range of where we stand, hold — the range-attack scan shoots back.
        const auto ct = character()->tile();
        if(ct) {
            const int curDist = std::max(abs(ttx - ct->x()),
                                         abs(tty - ct->y()));
            if(curDist <= range && curDist >= 1) return;
        }
        // Otherwise step to a firing tile near our slot from which the attacker
        // is in range. Hold if none — no chasing a far or moving attacker.
        const auto fire = sFindFiringTile(
            this, repositionAnchor(), ttx, tty, range, 2);
        if(!fire) return;
        setOverwrittableAction(false);
        goTo(fire->x(), fire->y(), 0);
        return;
    }
    // Melee: turn and close on whoever is hitting us. Already adjacent ⇒ the
    // adjacency scan locks next tick, hold. Else step onto the attacker tile
    // (dist 1). Bounded to the attacker's position (no open-ended chase), so a
    // back-rank soldier taking hits engages instead of standing in its slot.
    // Commit (non-overwrittable): a soldier hit by several foes gets a signal
    // per attacker; without this each one re-issues goTo to a different tile and
    // the soldier bounces, never reaching anyone. The gate at the top already
    // blocks re-entry once committed; cleared on arrival by goTo's finish.
    const auto ct = character()->tile();
    if(ct) {
        const int curDist = std::max(abs(ttx - ct->x()), abs(tty - ct->y()));
        if(curDist <= 1) return;
    }
    setOverwrittableAction(false);
    goTo(ttx, tty, 1);
}

void FightingAction::serializeFields(eSaveArchive& ar) {
    eComplexAction::serializeFields(ar);
    ar.field("angle", mAngle);
    ar.field("missile", mMissile);
    ar.field("meleeTime", mMeleeTime);
    ar.field("rangeAttack", mRangeAttack);
    ar.field("lookForEnemy", mLookForEnemy);
    ar.field("attackTime", mAttackTime);
    ar.field("attack", mAttack);
    ar.field("attackRanged", mAttackRanged, false);
    ar.archiveField("attackTarget", [this](eSaveArchive& targetAr) {
        mAttackTarget.serialize(targetAr, board());
    });
    ar.field("savedAction", mSavedAction);
    ar.field("overwrittableAction", mOverwrittableAction);
    ar.field("savedMove", mSavedMove, FightingSavedMove::none);
    ar.field("savedMoveX", mSavedMoveX, 0);
    ar.field("savedMoveY", mSavedMoveY, 0);
    ar.field("savedMoveDistance", mSavedMoveDistance, 0);
    ar.field("waitGoHomeRemaining", mWaitGoHomeRemaining, 0);
    ar.field("adjacencyCheck", mAdjacencyCheck, 0);
}

void FightingAction::resumeFromSavedState() {
    rebuildSavedRuntime();
}

bool FightingAction::atSavedMoveTarget() const {
    const auto c = character();
    const auto t = c->tile();
    if(!t) return true;
    return abs(mSavedMoveX - t->x()) <= mSavedMoveDistance &&
           abs(mSavedMoveY - t->y()) <= mSavedMoveDistance;
}

void FightingAction::rebuildSavedRuntime() {
    const auto c = character();
    if(mAttack) {
        if(!mAttackTarget.valid() || mAttackTarget.dead()) {
            mAttack = false;
            mAttackRanged = false;
            mAttackTarget.clear();
            mAttackTime = 0;
            c->setPlayFightSound(false);
            c->setActionType(mSavedAction);
        } else {
            c->setPlayFightSound(true);
            c->setActionType(mAttackRanged ? eCharacterActionType::fight2 :
                                             eCharacterActionType::fight);
            const vec2d cpos{c->absX(), c->absY()};
            const vec2d tpos{mAttackTarget.absX(), mAttackTarget.absY()};
            mAngle = (tpos - cpos).angle();
            c->setOrientation(sAngleOrientation(mAngle));
            return;
        }
    }
    if(mSavedMove == FightingSavedMove::waitGoHome) {
        if(mWaitGoHomeRemaining <= 0) {
            mSavedMove = FightingSavedMove::none;
            goHome();
        } else {
            waitAndGoHome(mWaitGoHomeRemaining);
        }
        return;
    }
    if(mSavedMove == FightingSavedMove::goTo) {
        if(atSavedMoveTarget()) {
            mSavedMove = FightingSavedMove::none;
            c->setActionType(eCharacterActionType::stand);
        } else {
            goTo(mSavedMoveX, mSavedMoveY, mSavedMoveDistance);
        }
        return;
    }
    eComplexAction::resumeFromSavedState();
}

void FightingAction::waitAndGoHome(const int w) {
    const auto c = character();
    c->setActionType(eCharacterActionType::none);
    mSavedMove = FightingSavedMove::waitGoHome;
    mWaitGoHomeRemaining = w;
    const auto finish = std::make_shared<SA_waitAndGoHomeFinish>(
        board(), this);
    const auto a = e::make_shared<eWaitAction>(c);
    a->setFinishAction(finish);
    a->setTime(w);
    setCurrentAction(a);
}
