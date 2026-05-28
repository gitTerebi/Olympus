#include "emonsteraction.h"

#include "engine/e-game-board.h"

#include "engine/boardData/eheatmaptask.h"
#include "buildings/eheatgetters.h"

#include "emovetoaction.h"
#include "ewaitaction.h"
#include "eiteratesquare.h"

#include "characters/gods/actions/god-action.h"

#include "enumbers.h"
#include "erand.h"
#include "fileIO/esavearchive.h"
#include "combat-timing.h"
#include "vec2.h"

eMonsterAction::eMonsterAction(eCharacter* const c) :
    eGodMonsterAction(c, eCharActionType::monsterAction),
    mType(eMonster::sCharacterToMonsterType(c->type())) {}

void eMonsterAction::increment(const int by) {
    const auto c = character();
    const auto at = c->actionType();
    if(mStage == eMonsterAttackStage::attacking &&
       mAttackActionType == eCharacterActionType::fight) {
        if(incrementMeleeAttack(by)) return;
    }
    if(at == eCharacterActionType::walk) {
        lookForAnyAttack(by, mLookForAttack,
                         attackPeriod(),
                         eNumbers::sMonsterAttackRange);
    }

    if(mStage == eMonsterAttackStage::wait && mWaitRemaining > 0) {
        mWaitRemaining -= by;
        if(mWaitRemaining <= 0) {
            mWaitRemaining = 0;
            setCurrentAction(nullptr);
        }
    }

    if(mStage == eMonsterAttackStage::patrol && mPatrolRemaining > 0) {
        mPatrolRemaining -= by;
        if(mPatrolRemaining <= 0) {
            mPatrolRemaining = 0;
            setCurrentAction(nullptr);
        }
    }

    eGodMonsterAction::increment(by);
}

bool eMonsterAction::decide() {
    const auto c = character();
    switch(mStage) {
    case eMonsterAttackStage::none:
        mStage = eMonsterAttackStage::wait;
        if(!c->tile()) {
            randomPlaceOnBoard();
            if(!c->tile()) c->kill();
        } else {
            mHomeTile = c->tile();
        }
        break;
    case eMonsterAttackStage::wait: {
        if(mWaitRemaining > 0) {
            rebuildWait();
            break;
        }
        mStage = eMonsterAttackStage::goTo;
        goToTarget();
        auto& board = this->board();
        board.updateMusic();
    } break;
    case eMonsterAttackStage::goTo:
        mStage = eMonsterAttackStage::patrol;
        monsterPatrol();
        break;
    case eMonsterAttackStage::patrol:
        mStage = eMonsterAttackStage::goBack;
        goBack();
        break;
    case eMonsterAttackStage::goBack: {
        mStage = eMonsterAttackStage::wait;
        enterWait();
        auto& board = this->board();
        board.updateMusic();
    } break;
    case eMonsterAttackStage::attacking:
        if(!mAttackTarget.target()) {
            finishAttack();
            return decide();
        }
        if(mAttackActionType == eCharacterActionType::fight) spawnMeleeAttack();
        else spawnAttackMissile();
        break;
    case eMonsterAttackStage::destroyingBuilding:
        if(!mAttackBuilding) {
            finishBuildingAttack();
            return decide();
        }
        spawnBuildingAttackMissiles();
        break;
    }
    return true;
}

void eMonsterAction::beginAttack(const eMissileTarget& target,
                                 const eCharacterActionType at,
                                 const eMonsterAttackStage prevStage) {
    mPreAttackStage = prevStage;
    mAttackTarget = target;
    mAttackActionType = at;
    mAttackTime = eMonster::sMonsterAttackTime(mType);
    mStage = eMonsterAttackStage::attacking;
    mLookForAttack = 0;
}

void eMonsterAction::finishAttack() {
    mStage = mPreAttackStage;
    mPreAttackStage = eMonsterAttackStage::none;
    mAttackTarget = eMissileTarget();
    mAttackTime = 0;
}

void eMonsterAction::spawnAttackMissile() {
    const auto c = character();
    const auto chart = c->type();
    const auto act = std::make_shared<eLookForAttackGodAct>(board(), c);
    const auto finishAttackA = std::make_shared<eMA_lookForRangeActionFinishAttack>(
                                   board(), this);
    spawnMissile(mAttackActionType, chart, mAttackTime,
                 mAttackTarget, nullptr, act, finishAttackA);
}

void eMonsterAction::spawnMeleeAttack() {
    const auto c = character();
    mAttackTime = CombatTiming::meleeAnimationMs(*c);
    if(c->actionType() != eCharacterActionType::fight) {
        c->setActionType(eCharacterActionType::fight);
    }
    const auto tt = mAttackTarget.target();
    if(tt) {
        const vec2d posdif{1. * tt->x() - c->absX(),
                           1. * tt->y() - c->absY()};
        c->setOrientation(sAngleOrientation(posdif.angle()));
    }
}

bool eMonsterAction::incrementMeleeAttack(const int by) {
    if(!mAttackTarget.target()) {
        finishAttack();
        resumeAction();
        if(!currentAction()) rebuildCurrentStage();
        return false;
    }
    const auto c = character();
    if(c->actionType() != eCharacterActionType::fight) {
        c->setActionType(eCharacterActionType::fight);
    }
    mAttackTime -= by;
    if(mAttackTime > 0) return true;

    if(const auto tc = mAttackTarget.character()) {
        if(!tc->dead()) tc->killWithCorpse();
    } else if(const auto tt = mAttackTarget.target()) {
        const auto b = tt->underBuilding();
        if(b) {
            b->collapse();
            eSounds::playCollapseSound();
        }
    }
    finishAttack();
    resumeAction();
    if(!currentAction()) rebuildCurrentStage();
    return true;
}

void eMonsterAction::spawnBuildingAttackMissiles() {
    const auto at = eCharacterActionType::fight2;
    const auto c = character();
    const auto chart = c->type();
    const auto b = mAttackBuilding.get();
    const auto finishAttackA = std::make_shared<eMA_destroyBuildingFinish>(
                                   board(), this, b);
    const auto playHitSound = std::make_shared<ePlayMonsterBuildingAttackSoundGodAct>(
                                  board(), b);
    spawnMultipleMissiles(at, chart, 500, b->centerTile(),
                          nullptr, playHitSound, finishAttackA, 3);
}

void eMonsterAction::serializeFields(eSaveArchive& ar) {
    eGodMonsterAction::serializeFields(ar);
    ar.tileField("homeTile", board(), mHomeTile);
    ar.field("aggressivness", mAggressivness);
    ar.field("stage", mStage);
    ar.field("lookForAttack", mLookForAttack);
    ar.field("preAttackStage", mPreAttackStage);
    missileTargetField(ar, "attackTarget", board(), mAttackTarget);
    ar.field("attackActionType", mAttackActionType);
    ar.field("attackTime", mAttackTime);
    ar.buildingField("attackBuilding", &board(), mAttackBuilding);
    ar.field("waitRemaining", mWaitRemaining);
    ar.field("patrolRemaining", mPatrolRemaining);
}

void eMonsterAction::resumeFromSavedState() {
    rebuildCurrentStage();
}

void eMonsterAction::rebuildCurrentStage() {
    if(state() != eCharacterActionState::running) return;
    switch(mStage) {
    case eMonsterAttackStage::attacking:
        if(!mAttackTarget.target()) {
            finishAttack();
            rebuildCurrentStage();
            return;
        }
        if(mAttackActionType == eCharacterActionType::fight) spawnMeleeAttack();
        else spawnAttackMissile();
        return;
    case eMonsterAttackStage::destroyingBuilding:
        if(!mAttackBuilding) {
            finishBuildingAttack();
            rebuildCurrentStage();
            return;
        }
        spawnBuildingAttackMissiles();
        return;
    case eMonsterAttackStage::wait:
        if(mWaitRemaining > 0) rebuildWait();
        else eGodMonsterAction::resumeFromSavedState();
        return;
    case eMonsterAttackStage::goTo:
        goToTarget();
        return;
    case eMonsterAttackStage::patrol:
        if(mPatrolRemaining > 0) rebuildMonsterPatrol();
        else monsterPatrol();
        return;
    case eMonsterAttackStage::goBack:
        goBack();
        return;
    case eMonsterAttackStage::none:
        eGodMonsterAction::resumeFromSavedState();
        return;
    }
}

eTile* eMonsterAction::closestEmptySpace(const int rdx, const int rdy) const {
    const auto c = character();
    const auto cid = c->onCityId();
    auto& board = c->getBoard();
    eTile* plainTile = nullptr;
    const auto prcsTile = [&](const int i, const int j) {
        const int tx = rdx + i;
        const int ty = rdy + j;
        const auto tt = board.tile(tx, ty);
        if(!tt) return false;
        const auto ttcid = tt->cityId();
        if(ttcid != cid) return false;
        if(!plainTile && tt->walkable()) {
            plainTile = tt;
            return true;
        }
        return false;
    };

    for(int k = 0; k < 1000; k++) {
        eIterateSquare::iterateSquare(k, prcsTile, 1);
    }

    return plainTile;
}

void eMonsterAction::randomPlaceOnBoard() {
    const auto c = character();
    const auto cid = c->onCityId();
    auto& board = c->getBoard();
    const auto city = board.boardCityWithId(cid);
    const auto rtile = city->randomTile();
    const int tx = rtile->x();
    const int ty = rtile->y();
    const auto tile = closestEmptySpace(tx, ty);
    if(!tile) return;
    c->changeTile(tile);
    mHomeTile = tile;
//    const auto c = character();
//    const stdptr<eMonsterAction> tptr(this);
//    const stdptr<eCharacter> cptr(c);
//    const auto hmFinish = [tptr, this, cptr, c](eHeatMap& map) {
//        if(!tptr || !cptr) return;
//        eHeatMapDivisor divisor(map);
//        divisor.divide(10);
//        int tx;
//        int ty;
//        const bool r = divisor.maxHeatTile(tx, ty);
//        if(r) {
//            const auto& board = c->getBoard();
//            c->changeTile(board.tile(tx, ty));
//        } else {
//            setCurrentAction(nullptr);
//        }
//    };
//    const auto hg = eHeatGetters::empty;
//    const auto task = new eHeatMapTask(hg, hmFinish);
//    auto& board = c->getBoard();
//    auto& tp = board.threadPool();
//    tp.queueTask(task);
    //    wait();
}

stdsptr<eObsticleHandler> eMonsterAction::obsticleHandler() {
    return std::make_shared<eMonsterObsticleHandler>(board(), this);
}

void eMonsterAction::goToTarget() {
    if(mType == eMonsterType::scylla ||
       mType == eMonsterType::kraken) {
        const auto c = character();

        const auto underBuilding = [](eTileBase* const tile) {
            const auto type = tile->underBuildingType();
            return eBuilding::sAttackable(type);
        };

        const auto a = e::make_shared<eMoveToAction>(c);
        a->setStateRelevance(eStateRelevance::buildings |
                             eStateRelevance::terrain);
        const stdptr<eMonsterAction> tptr(this);
        a->setFoundAction([tptr, this]() {
            if(!tptr) return;
            const auto c = character();
            c->setActionType(eCharacterActionType::walk);
        });
        a->setRemoveLastTurn(true);

        a->start(underBuilding, eWalkableObject::sCreateDeepWater());
        setCurrentAction(a);
    } else {
        const stdptr<eMonsterAction> tptr(this);
        const auto tryAgain = std::make_shared<eGoToTargetTryAgain>(
                                  board(), this);
        eGodMonsterAction::goToTarget(eHeatGetters::any, tryAgain,
                                      obsticleHandler(),
                                      eWalkableHelpers::sMonsterTileDistance,
                                      eWalkableObject::sCreateAttacker(),
                                      eWalkableObject::sCreateDefault());
    }
}

void eMonsterAction::goBack() {
    if(!mHomeTile) return setCurrentAction(nullptr);
    const auto c = character();

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setTileDistance(eWalkableHelpers::sMonsterTileDistance);
    a->setObsticleHandler(obsticleHandler());
    a->setFindFailAction([](){});

    if(mType == eMonsterType::scylla ||
       mType == eMonsterType::kraken) {
        a->start(mHomeTile, eWalkableObject::sCreateDeepWater());
    } else {
        a->start(mHomeTile, eWalkableObject::sCreateAttacker(),
                 eWalkableObject::sCreateDefault());
    }
    setCurrentAction(a);
    c->setActionType(eCharacterActionType::walk);
}

void eMonsterAction::monsterPatrol() {
    enterMonsterPatrol();
}

void eMonsterAction::enterMonsterPatrol() {
    if(mType == eMonsterType::scylla ||
       mType == eMonsterType::kraken) {
        mPatrolRemaining = moveAroundPeriod();
        rebuildMonsterPatrol();
    } else {
        mPatrolRemaining = 0;
        goToNearestRoad();
    }
}

void eMonsterAction::rebuildMonsterPatrol() {
    if(mType == eMonsterType::scylla ||
       mType == eMonsterType::kraken) {
        moveAround(nullptr, mPatrolRemaining, eWalkableObject::sCreateDeepWater());
    } else {
        goToNearestRoad();
    }
}

void eMonsterAction::destroyBuilding(eBuilding* const b) {
    const auto at = eCharacterActionType::fight2;
    const auto c = character();
    const auto chart = c->type();
    const auto finishAttackA = std::make_shared<eMA_destroyBuildingFinish>(
                                   board(), this, b);

    const auto playHitSound = std::make_shared<ePlayMonsterBuildingAttackSoundGodAct>(
                                  board(), b);
    pauseAction();
    beginBuildingAttack(b, mStage);
    spawnMultipleMissiles(at, chart, 500, b->centerTile(),
                          nullptr, playHitSound, finishAttackA, 3);
}

void eMonsterAction::beginBuildingAttack(eBuilding* const b,
                                         const eMonsterAttackStage prevStage) {
    mPreAttackStage = prevStage;
    mAttackBuilding = b;
    mStage = eMonsterAttackStage::destroyingBuilding;
    mLookForAttack = 0;
}

void eMonsterAction::finishBuildingAttack() {
    mStage = mPreAttackStage;
    mPreAttackStage = eMonsterAttackStage::none;
    mAttackBuilding = nullptr;
}

void eMonsterAction::enterWait() {
    mWaitRemaining = invadePeriod();
    rebuildWait();
}

void eMonsterAction::rebuildWait() {
    if(mType == eMonsterType::scylla ||
       mType == eMonsterType::kraken) {
        moveAround(nullptr, mWaitRemaining,
                   eWalkableObject::sCreateDeepWater());
    } else {
        moveAround(nullptr, mWaitRemaining);
    }
}

bool eMonsterAction::lookForAttack(const int dtime,
                                   int& time, const int freq,
                                   const int range) {
    const auto c = character();
    const auto act = std::make_shared<eLookForAttackGodAct>(
                         board(), c);

    const auto at = eCharacterActionType::fight2;

    return lookForRangeAction(dtime, time, freq, range,
                              at, act, nullptr);
}

bool eMonsterAction::lookForMeleeAttack(const bool charactersOnly,
                                        const bool buildingsOnly) {
    const auto c = character();
    const auto act = std::make_shared<eLookForAttackGodAct>(
                         board(), c);
    auto& brd = c->getBoard();
    const auto ct = c->tile();
    if(!ct) return false;
    const int tx = ct->x();
    const int ty = ct->y();
    std::vector<eTile*> tiles;
    tiles.reserve(9);
    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
            const auto t = brd.tile(tx + i, ty + j);
            if(t) tiles.push_back(t);
        }
    }
    std::random_shuffle(tiles.begin(), tiles.end());
    for(const auto t : tiles) {
        const auto tt = act->find(t);
        if(!tt) continue;
        if(charactersOnly && !tt.character()) continue;
        if(buildingsOnly && tt.character()) continue;
        pauseAction();
        beginAttack(tt, eCharacterActionType::fight, mStage);
        spawnMeleeAttack();
        return true;
    }
    return false;
}

bool eMonsterAction::lookForAnyAttack(const int dtime,
                                      int& time, const int freq,
                                      const int range) {
    time += dtime;
    if(time <= freq) return false;
    time -= freq;
    if(eRand::rand() % 10 == 0) {
        const auto c = character();
        pauseAction();
        c->setActionType(eCharacterActionType::stand);
        board().ifVisible(c->tile(), [&]() {
            eSounds::playMonsterSound(mType, eMonsterSound::voice);
        });
        const auto w = e::make_shared<eWaitAction>(c);
        const auto finish = std::make_shared<eMA_lookForRangeActionFinishAttack>(
                                board(), this);
        w->setFinishAction(finish);
        w->setFailAction(finish);
        w->setTime(700);
        setCurrentAction(w);
        return true;
    }
    const auto rangedBuilding = [this, &time, range]() {
        return lookForRangeAction(
            0, time, 0, range, eCharacterActionType::fight2,
            std::make_shared<eLookForAttackGodAct>(board(), character()),
            nullptr, true);
    };
    const int roll = eRand::rand() % 3;
    for(int i = 0; i < 3; i++) {
        const int choice = (roll + i) % 3;
        if(choice == 0 && lookForMeleeAttack(true)) return true;
        if(choice == 1 && lookForMeleeAttack(false, true)) return true;
        if(choice == 2 && rangedBuilding()) return true;
    }
    return false;
}

bool eMonsterAction::lookForRangeAction(const int dtime,
                                        int& time, const int freq,
                                        const int range,
                                        const eCharacterActionType at,
                                        const stdsptr<eGodAct>& act,
                                        const stdsptr<eCharActFunc>& missileSound,
                                        const bool buildingsOnly) {
    const auto c = character();
    const auto chart = c->type();
    const auto cat = c->actionType();
    const bool walking = cat == eCharacterActionType::walk;
    if(!walking) return false;
    auto& brd = c->getBoard();
    const auto ct = c->tile();
    if(!ct) return false;
    const int tx = ct->x();
    const int ty = ct->y();

    time += dtime;
    if(time > freq) {
        time -= freq;
        std::vector<eTile*> tiles;
        const int rr = 2*range + 1;
        tiles.reserve(rr*rr);
        for(int i = -range; i <= range; i++) {
            for(int j = -range; j <= range; j++) {
                const int ttx = tx + i;
                const int tty = ty + j;
                const auto t = brd.tile(ttx, tty);
                if(!t) continue;
                tiles.push_back(t);
            }
        }
        std::random_shuffle(tiles.begin(), tiles.end());
        for(const auto t : tiles) {
            const auto tt = act->find(t);
            if(!tt) continue;
            if(buildingsOnly && tt.character()) continue;
            if(buildingsOnly && std::abs(t->x() - tx) <= 1 &&
               std::abs(t->y() - ty) <= 1) {
                continue;
            }
            const auto targetTile = tt.target();
            if(buildingsOnly && targetTile &&
               std::abs(targetTile->x() - tx) <= 1 &&
               std::abs(targetTile->y() - ty) <= 1) {
                continue;
            }

            const auto finishAttackA = std::make_shared<eMA_lookForRangeActionFinishAttack>(
                                           board(), this);

            pauseAction();
            beginAttack(eMissileTarget(tt), at, mStage);
            const int attackTime = eMonster::sMonsterAttackTime(mType);
            spawnMissile(at, chart, attackTime, tt,
                         missileSound, act, finishAttackA);
            return true;
        }
    }
    return false;
}

int eMonsterAction::attackPeriod() const {
    switch(mAggressivness) {
    case eMonsterAggressivness::passive:
        return eNumbers::sPassiveMonsterAttackPeriod;
    case eMonsterAggressivness::active:
        return eNumbers::sActiveMonsterAttackPeriod;
    case eMonsterAggressivness::veryActive:
        return eNumbers::sVeryActiveMonsterAttackPeriod;
    case eMonsterAggressivness::aggressive:
        return eNumbers::sAggressiveMonsterAttackPeriod;
    }
    return eNumbers::sPassiveMonsterAttackPeriod;
}

int eMonsterAction::invadePeriod() const {
    if(mType == eMonsterType::scylla ||
       mType == eMonsterType::kraken) {
        switch(mAggressivness) {
        case eMonsterAggressivness::passive:
            return eNumbers::sPassiveWaterMonsterInvadePeriod;
        case eMonsterAggressivness::active:
            return eNumbers::sActiveWaterMonsterInvadePeriod;
        case eMonsterAggressivness::veryActive:
            return eNumbers::sVeryActiveWaterMonsterInvadePeriod;
        case eMonsterAggressivness::aggressive:
            return eNumbers::sAggressiveWaterMonsterInvadePeriod;
        }
        return eNumbers::sPassiveWaterMonsterInvadePeriod;
    } else {
        switch(mAggressivness) {
        case eMonsterAggressivness::passive:
            return eNumbers::sPassiveLandMonsterInvadePeriod;
        case eMonsterAggressivness::active:
            return eNumbers::sActiveLandMonsterInvadePeriod;
        case eMonsterAggressivness::veryActive:
            return eNumbers::sVeryActiveLandMonsterInvadePeriod;
        case eMonsterAggressivness::aggressive:
            return eNumbers::sAggressiveLandMonsterInvadePeriod;
        }
        return eNumbers::sPassiveLandMonsterInvadePeriod;
    }
}

int eMonsterAction::moveAroundPeriod() const {
    if(mType == eMonsterType::scylla ||
        mType == eMonsterType::kraken) {
        switch(mAggressivness) {
        case eMonsterAggressivness::passive:
            return eNumbers::sPassiveWaterMonsterMoveAroundPeriod;
        case eMonsterAggressivness::active:
            return eNumbers::sActiveWaterMonsterMoveAroundPeriod;
        case eMonsterAggressivness::veryActive:
            return eNumbers::sVeryActiveWaterMonsterMoveAroundPeriod;
        case eMonsterAggressivness::aggressive:
            return eNumbers::sAggressiveWaterMonsterMoveAroundPeriod;
        }
        return eNumbers::sPassiveWaterMonsterMoveAroundPeriod;
    } else {
        switch(mAggressivness) {
        case eMonsterAggressivness::passive:
            return eNumbers::sPassiveLandMonsterMoveAroundPeriod;
        case eMonsterAggressivness::active:
            return eNumbers::sActiveLandMonsterMoveAroundPeriod;
        case eMonsterAggressivness::veryActive:
            return eNumbers::sVeryActiveLandMonsterMoveAroundPeriod;
        case eMonsterAggressivness::aggressive:
            return eNumbers::sAggressiveLandMonsterMoveAroundPeriod;
        }
        return eNumbers::sPassiveLandMonsterMoveAroundPeriod;
    }
}
