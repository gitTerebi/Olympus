#include "fighting-action.h"

#include <math.h>
#include <cstdio>
#include <algorithm>

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

void FightingAction::cancelAttack() {
    mAttack = false;
    mAttackRanged = false;
    mAttackTarget.clear();
    mAttackTime = 0;
    mMeleeTime = 0;
    mMissile = 0;
    const auto c = character();
    c->setPlayFightSound(false);
    c->setActionType(mSavedAction);
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
    const int rangeAttackCheck = 500;
    const int lookForEnemyCheck = 500;
    const int buildingCheck = 5000;

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
                    const bool d = mAttackTarget.takeDamage(att, c);
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
                const bool d = mAttackTarget.takeMeleeDamage(per, c);
                if(d) finishAttack = true;
            }
        }
        if(finishAttack) {
            mAttack = false;
            mAttackTarget.clear();
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

    mBuildingAttack += by;
    const bool buildingAttack = mBuildingAttack > buildingCheck;
    if(buildingAttack) {
        mBuildingAttack -= buildingCheck;
    }
    stdsptr<eCharacter> secondOption;
    stdsptr<eCharacter> thirdOption;
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
                const vec2d ccpos{cc->absX(), cc->absY()};
                const vec2d posdif = ccpos - cpos;
                const double dist = posdif.length();
                if(dist > 1.) continue;
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
        setAttackTarget(cc, false);
        return LookForEnemyState::attacking;
            }
            if(buildingAttack) {
                const bool r = attackBuilding(t, false);
                if(r) return LookForEnemyState::attacking;
            }
        }
    }
    if(secondOption) {
        setAttackTarget(secondOption, false);
        return LookForEnemyState::attacking;
    }
    if(thirdOption) {
        setAttackTarget(thirdOption, false);
        return LookForEnemyState::attacking;
    }

    if(range > 0) {
        mRangeAttack += by;
        if(mRangeAttack > rangeAttackCheck) {
            mRangeAttack -= rangeAttackCheck;
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
                         setAttackTarget(cc, true);
                         sSignalBeingAttack(cc.get(), c, brd);
                         return LookForEnemyState::attacking;
                    }
                    if(buildingAttack) {
                        const bool r = attackBuilding(t, true);
                        if(r) return LookForEnemyState::attacking;
                    }
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

    if(!currentAction() || mOverwrittableAction) {
        mLookForEnemy += by;
        if(mLookForEnemy > lookForEnemyCheck) {
            mLookForEnemy -= lookForEnemyCheck;
            // Spot the nearest enemy a short way out. Melee units (range == 0)
            // chase it; ranged units do NOT — they only use its position to
            // pick a firing tile near their own slot (see below).
            // Wide detect: the unit notices distant enemies but only takes a few
            // steps toward them (walk spread below). Stays put and waits for the
            // enemy to close rather than charging out.
            const int erange = sRangedDetectRange(range);
            for(int i = -erange; i <= erange; i++) {
                for(int j = -erange; j <= erange; j++) {
                    const int ttx = tx + i;
                    const int tty = ty + j;
                    const auto t = brd.tile(ttx, tty);
                    if(!t) continue;
                    const auto& chars = t->characters();
                    for(const auto& cc : chars) {
                        if(!sCanAttackCharacter(cc.get())) continue;
                        const auto cctid = cc->teamId();
                        if(!eTeamIdHelpers::isEnemy(cctid, tid)) continue;
                        if(cc->dead()) continue;
                        if(range > 0) {
                            // Already within firing range of where we stand:
                            // hold and let the range-attack scan above shoot.
                            // Repositioning here is what made rabble step forward
                            // then walk back without ever firing.
                            const int curDist = std::max(abs(ttx - tx),
                                                         abs(tty - ty));
                            if(curDist <= range && curDist >= 1) {
                                return LookForEnemyState::attacking;
                            }
                            // Out of range: step at most 4 tiles from the
                            // formation slot to a tile that brings this enemy
                            // into firing range, then hold and shoot. Never walk
                            // to the enemy tile. If none works (enemy too far),
                            // stay put and wait for it to come closer.
                            const auto fire = sFindFiringTile(
                                this, repositionAnchor(), ttx, tty, range, 4);
                            if(!fire) continue;
                            setOverwrittableAction(false);
                            goTo(fire->x(), fire->y(), 0);
                            return LookForEnemyState::attacking;
                        }
                        // Melee: close in on the enemy as before.
                        setOverwrittableAction(false);
                        goTo(ttx, tty, 0);
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

    const auto tcid = t->cityId();
    auto& board = this->board();
    const auto ttid = board.cityIdToTeamId(tcid);
    const auto tid = c->teamId();
    const bool attackBuildings = eTeamIdHelpers::isEnemy(tid, ttid);
    stdsptr<eWalkableObject> pathFindWalkable;
    stdsptr<eWalkableObject> moveWalkable;
    if(c->isBoat()) {
        pathFindWalkable =
            eWalkableObject::sCreateDeepWater();
    } else {
        if(attackBuildings) {
            pathFindWalkable = eWalkableObject::sCreateAttacker();
            moveWalkable = eWalkableObject::sCreateDefault();
        } else {
            pathFindWalkable =
                eWalkableObject::sCreateDefault();
        }
    }

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setFailAction(finishAct);
    a->setFinishAction(finishAct);
    a->setFindFailAction(findFailAct);
    if(attackBuildings) {
        a->setObsticleHandler(obsticleHandler());
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

void FightingAction::beingAttacked(const int ttx, const int tty) {
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
        // Otherwise generate stand tiles around our own formation slot (<= 2
        // away), pick one from which the attacker is within firing range, and
        // move there. Hold if none — no chasing a far or moving attacker.
        const auto fire = sFindFiringTile(
            this, repositionAnchor(), ttx, tty, range, 2);
        if(!fire) return;
        setOverwrittableAction(false);
        goTo(fire->x(), fire->y(), 0);
        return;
    }
    setOverwrittableAction(false);
    goTo(ttx, tty, 0);
}

void FightingAction::serializeFields(eSaveArchive& ar) {
    eComplexAction::serializeFields(ar);
    ar.field("angle", mAngle);
    ar.field("missile", mMissile);
    ar.field("meleeTime", mMeleeTime);
    ar.field("rangeAttack", mRangeAttack);
    ar.field("buildingAttack", mBuildingAttack);
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
