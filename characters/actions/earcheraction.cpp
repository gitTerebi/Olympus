#include "earcheraction.h"

#include "epatrolmoveaction.h"

#include "characters/echaracter.h"
#include "buildings/ebuilding.h"
#include "engine/etile.h"
#include "engine/e-game-board.h"
#include "vec2.h"
#include "characters/esoldier.h"
#include "characters/actions/soldier-action.h"
#include "characters/earcher.h"

#include "missiles/earrowmissile.h"

#include "edieaction.h"
#include "audio/sounds.h"
#include "enumbers.h"
#include "fileIO/esavearchive.h"

eArcherAction::eArcherAction(eCharacter* const c) :
    eComplexAction(c, eCharActionType::archerAction) {}

void eArcherAction::increment(const int by) {
    const int rangeAttackCheck = 500;
    const auto cc = character();
    const int range = cc->range() > 0 ? cc->range() : eNumbers::sWallArcherRange;
    const int missileCheck = cc->missileFreq() > 0 ? cc->missileFreq() * 10 : 200;

    const auto c = character();
    const auto ct = c->tile();
    const int tx = ct->x();
    const int ty = ct->y();
    const vec2d cpos{c->absX(), c->absY()};
    const auto tid = c->teamId();
    auto& brd = c->getBoard();
    
    // Check if wall archer is still on a valid wall position.
    // When the 2x2 wall block is broken, the archer falls to ground.
    const auto checker = [](eTileBase* const t) {
        if(!t) return false;
        const auto ubt = t->underBuildingType();
        return ubt == eBuildingType::wall ||
               ubt == eBuildingType::tower;
    };
    if(!checker(ct) || !checker(ct->tileRel(0, 1)) ||
       !checker(ct->tileRel(1, 0)) || !checker(ct->tileRel(1, 1))) {
        c->kill();
        return;
    }

    if(mAttack) {
        bool finishAttack = false;
        if(range > 0 && mAttackTarget) {
            mMissile += by;
            if(mMissile > missileCheck) {
                mMissile = mMissile - missileCheck;
                const auto tt = mAttackTarget->tile();
                const int ttx = tt->x();
                const int tty = tt->y();
                eMissile::sCreate<eArrowMissile>(brd, tx, ty, 0.5,
                                                 ttx, tty, 0.5, 2);
                auto& board = this->board();
                board.ifVisible(c->tile(), [&]() {
                    eSounds::playAttackSound(c);
                });
                if(!mAttackTarget->dead()) {
                    const double arm = mAttackTarget->armorVsMissiles();
                    const double ma = c->missileAttack() > 0 ? c->missileAttack() : c->attack();
                    const double dmg = ma - arm;
                    const double att = dmg > 0 ? dmg : 0.01;
                    const bool d = mAttackTarget->takeDamage(att, c);
                    if(d) {
                        const auto a = e::make_shared<eDieAction>(mAttackTarget);
                        mAttackTarget->setAction(a);
                        finishAttack = true;
                    }
                }
            }
        }
        mAttackTime += by;
        if(!finishAttack) finishAttack = !mAttackTarget ||
                                          mAttackTarget->dead() ||
                                          mAttackTime > 1000;
        if(finishAttack) {
            mAttack = false;
            mAttackTarget = nullptr;
            mAttackTime = 0;
            mRangeAttack = rangeAttackCheck;
            c->setActionType(eCharacterActionType::walk);
        } else {
            return;
        }
    }

    mRangeAttack += by;
    if(mRangeAttack > rangeAttackCheck) {
        mRangeAttack = mRangeAttack - rangeAttackCheck;
        for(int i = -range; i <= range; i++) {
            for(int j = -range; j <= range; j++) {
                const auto t = brd.tile(tx + i, ty + j);
                if(!t) continue;
                const auto& chars = t->characters();
                for(const auto& cc : chars) {
                    if(!cc->isSoldier()) continue;
                    const auto cctid = cc->teamId();
                    if(!eTeamIdHelpers::isEnemy(cctid, tid)) continue;
                    if(cc->dead()) continue;
                    const vec2d ccpos{cc->absX(), cc->absY()};
                    const vec2d posdif = ccpos - cpos;
                    mAttackTarget = cc;
                    mAttack = true;
                    mAttackTime = 0;
                    c->setActionType(eCharacterActionType::fight);
                    const double angle = posdif.angle();
                    const auto o = sAngleOrientation(angle);
                    c->setOrientation(o);

                    const auto tt = cc->tile();
                    const int ttx = tt->x();
                    const int tty = tt->y();
                    eFightingAction::sSignalBeingAttack(cc.get(), ttx, tty, brd);

                    return;
                }
            }
        }
    }
    eComplexAction::increment(by);
}

bool eArcherAction::decide() {
    const auto c = character();
    c->setActionType(eCharacterActionType::walk);
    const auto fail = std::make_shared<eAA_patrolFail>(board(), this);
    const auto finish = std::make_shared<eAA_patrolFinish>(board(), this);
    const auto a = e::make_shared<ePatrolMoveAction>(
                       c, false, eWalkableObject::sCreateWall());
    a->setFinishAction(fail);
    a->setFinishAction(finish);
    setCurrentAction(a);
    return true;
}

void eArcherAction::serializeFields(eSaveArchive& ar) {
    eComplexAction::serializeFields(ar);
    ar.field("missile", mMissile);
    ar.field("rangeAttack", mRangeAttack);
    ar.field("attackTime", mAttackTime);
    ar.field("attack", mAttack);
    ar.characterField("attackTarget", &board(), mAttackTarget);
}

void eArcherAction::resumeFromSavedState() {
    if(mAttack && mAttackTarget && !mAttackTarget->dead()) {
        character()->setActionType(eCharacterActionType::fight);
    } else {
        mAttack = false;
        mAttackTarget = nullptr;
        eComplexAction::resumeFromSavedState();
    }
}
