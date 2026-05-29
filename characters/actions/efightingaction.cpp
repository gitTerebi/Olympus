#include "efightingaction.h"

#include <math.h>
#include <cstdio>

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
}

eAttackTarget::eAttackTarget() :
    mC(nullptr), mB(nullptr) {}

eAttackTarget::eAttackTarget(eCharacter* const c) :
    mC(c), mB(nullptr) {}

eAttackTarget::eAttackTarget(eBuilding* const b) :
    mC(nullptr), mB(b) {}

eTile* eAttackTarget::tile() const {
    if(mC) return mC->tile();
    if(mB) return mB->centerTile();
    return nullptr;
}

bool eAttackTarget::valid() const {
    return mC || mB;
}

bool eAttackTarget::takeDamage(const double a, eCharacter* const attacker) {
    if(mC) return mC->takeDamage(a, attacker);
    if(mB) return mB->takeDamage(a);
    return true;
}

bool eAttackTarget::takeMeleeDamage(const double a, eCharacter* const attacker) {
    if(mC) return mC->takeMeleeDamage(a, attacker);
    if(mB) return mB->takeDamage(a);
    return true;
}

bool eAttackTarget::dead() const {
    if(mC) return mC->dead();
    if(mB) return false;
    return true;
}

void eAttackTarget::clear() {
    mC = nullptr;
    mB = nullptr;
}

int eAttackTarget::armor() const {
    if(mC) return mC->armor();
    return 0;
}

int eAttackTarget::armorVsMissiles() const {
    if(mC) return mC->armorVsMissiles();
    return 0;
}

bool eAttackTarget::building() const {
    return mB;
}

double eAttackTarget::absX() const {
    if(mC) return mC->absX();
    if(mB) {
        const auto t = mB->centerTile();
        if(!t) return 0.;
        return t->x();
    }
    return 0.;
}

double eAttackTarget::absY() const {
    if(mC) return mC->absY();
    if(mB) {
        const auto t = mB->centerTile();
        if(!t) return 0.;
        return t->y();
    }
    return 0.;
}

void eAttackTarget::serialize(eSaveArchive& ar, GameBoard& board) {
    ar.characterField("character", &board, mC);
    ar.buildingField("building", &board, mB);
}

void eFightingAction::cancelAttack() {
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

void eFightingAction::sSignalBeingAttack(
    eCharacter* const attacked,
    eCharacter* const by,
    GameBoard& brd) {
    const auto tt = by->tile();
    if(!tt) return;
    const int ttx = tt->x();
    const int tty = tt->y();
    sSignalBeingAttack(attacked, ttx, tty, brd);
}

void eFightingAction::sSignalBeingAttack(eCharacter * const attacked,
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

eLookForEnemyState eFightingAction::lookForEnemy(const int by) {
    if(mSavedMove == eFightingSavedMove::waitGoHome) {
        mWaitGoHomeRemaining -= by;
        if(mWaitGoHomeRemaining <= 0) {
            mWaitGoHomeRemaining = 0;
            mSavedMove = eFightingSavedMove::none;
            setCurrentAction(nullptr);
            goHome();
        }
        return eLookForEnemyState::none;
    }
    const int rangeAttackCheck = 500;
    const int lookForEnemyCheck = 500;
    const int buildingCheck = 5000;

    const auto c = character();
    if(c->dead()) return eLookForEnemyState::dead;
    int range = c->range();
    auto& brd = c->getBoard();
    const auto ct = c->tile();
    if(!ct) return eLookForEnemyState::dead;
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
            return eLookForEnemyState::attacking;
        }
    }
    const vec2d cpos{c->absX(), c->absY()};

    const auto setAttackTarget = [&](const stdsptr<eCharacter>& cc,
                                      const bool range) {
        const vec2d ccpos{cc->absX(), cc->absY()};
        const vec2d posdif = ccpos - cpos;
        mAttackTarget = eAttackTarget(cc.get());
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
        return eLookForEnemyState::attacking;
            }
            if(buildingAttack) {
                const bool r = attackBuilding(t, false);
                if(r) return eLookForEnemyState::attacking;
            }
        }
    }
    if(secondOption) {
        setAttackTarget(secondOption, false);
        return eLookForEnemyState::attacking;
    }
    if(thirdOption) {
        setAttackTarget(thirdOption, false);
        return eLookForEnemyState::attacking;
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
                         return eLookForEnemyState::attacking;
                    }
                    if(buildingAttack) {
                        const bool r = attackBuilding(t, true);
                        if(r) return eLookForEnemyState::attacking;
                    }
                }
            }
        }

        if(secondOption) {
            setAttackTarget(secondOption, true);
            return eLookForEnemyState::attacking;
        }
        if(thirdOption) {
            setAttackTarget(thirdOption, true);
            return eLookForEnemyState::attacking;
        }
    }

    if(!currentAction() || mOverwrittableAction) {
        mLookForEnemy += by;
        if(mLookForEnemy > lookForEnemyCheck) {
            mLookForEnemy -= lookForEnemyCheck;
            const int erange = 3 + range;
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
                        setOverwrittableAction(false);
                        goTo(ttx, tty, range);
                        return eLookForEnemyState::attacking;
                    }
                }
            }
        }
    }

    return eLookForEnemyState::none;
}

bool eFightingAction::attackBuilding(eTile* const t, const bool range) {
    const auto ub = t->underBuilding();
    if(!ub) return false;
    const auto c = character();
    const auto tid = c->teamId();
    if(!eTeamIdHelpers::isEnemy(ub->teamId(), tid)) return false;
    const vec2d cpos{c->absX(), c->absY()};
    const bool att = eBuilding::sAttackable(ub->type());
    if(!att) return false;
    mAttackTarget = eAttackTarget(ub);
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

void eFightingAction::goTo(const int fx, const int fy,
                           const int dist,
                           const eAction& findFailAct,
                           const eAction& findFinishAct) {
    const auto c = character();
    const auto t = c->tile();
    const int sx = t->x();
    const int sy = t->y();
    if(abs(fx - sx) <= dist && abs(fy - sy) <= dist) {
        mSavedMove = eFightingSavedMove::none;
        return;
    }
    mSavedMove = eFightingSavedMove::goTo;
    mSavedMoveX = fx;
    mSavedMoveY = fy;
    mSavedMoveDistance = dist;

    const auto hha = [fx, fy, dist](eThreadTile* const t) {
        return abs(t->x() - fx) <= dist && abs(t->y() - fy) <= dist;
    };

    const auto finishAct = std::make_shared<eSA_goToFinish>(
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
    const stdptr<eFightingAction> tptr(this);
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

void eFightingAction::beingAttacked(eCharacter* const ss) {
    const auto tt = ss->tile();
    const int ttx = tt->x();
    const int tty = tt->y();
    beingAttacked(ttx, tty);
}

void eFightingAction::beingAttacked(const int ttx, const int tty) {
    if(mAttack) return;
    if(!mOverwrittableAction && currentAction()) return;
    setOverwrittableAction(false);
    goTo(ttx, tty);
}

void eFightingAction::serializeFields(eSaveArchive& ar) {
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
    ar.field("savedMove", mSavedMove, eFightingSavedMove::none);
    ar.field("savedMoveX", mSavedMoveX, 0);
    ar.field("savedMoveY", mSavedMoveY, 0);
    ar.field("savedMoveDistance", mSavedMoveDistance, 0);
    ar.field("waitGoHomeRemaining", mWaitGoHomeRemaining, 0);
}

void eFightingAction::resumeFromSavedState() {
    rebuildSavedRuntime();
}

bool eFightingAction::atSavedMoveTarget() const {
    const auto c = character();
    const auto t = c->tile();
    if(!t) return true;
    return abs(mSavedMoveX - t->x()) <= mSavedMoveDistance &&
           abs(mSavedMoveY - t->y()) <= mSavedMoveDistance;
}

void eFightingAction::rebuildSavedRuntime() {
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
    if(mSavedMove == eFightingSavedMove::waitGoHome) {
        if(mWaitGoHomeRemaining <= 0) {
            mSavedMove = eFightingSavedMove::none;
            goHome();
        } else {
            waitAndGoHome(mWaitGoHomeRemaining);
        }
        return;
    }
    if(mSavedMove == eFightingSavedMove::goTo) {
        if(atSavedMoveTarget()) {
            mSavedMove = eFightingSavedMove::none;
            c->setActionType(eCharacterActionType::stand);
        } else {
            goTo(mSavedMoveX, mSavedMoveY, mSavedMoveDistance);
        }
        return;
    }
    eComplexAction::resumeFromSavedState();
}

void eFightingAction::waitAndGoHome(const int w) {
    const auto c = character();
    c->setActionType(eCharacterActionType::none);
    mSavedMove = eFightingSavedMove::waitGoHome;
    mWaitGoHomeRemaining = w;
    const auto finish = std::make_shared<eSA_waitAndGoHomeFinish>(
        board(), this);
    const auto a = e::make_shared<eWaitAction>(c);
    a->setFinishAction(finish);
    a->setTime(w);
    setCurrentAction(a);
}
