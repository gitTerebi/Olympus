#include "monster-action.h"

#include "engine/game-board.h"

#include "engine/boardData/eheatmaptask.h"
#include "buildings/eheatgetters.h"

#include "emovetoaction.h"
#include "eiteratesquare.h"

#include "characters/gods/actions/god-action.h"

#include "enumbers.h"
#include "erand.h"
#include "fileIO/esavearchive.h"
#include "combat-timing.h"
#include "vec2.h"

MonsterAction::MonsterAction(eCharacter* const c) :
    GodMonsterAction(c, eCharActionType::monsterAction),
    mType(eMonster::sCharacterToMonsterType(c->type())) {}

void MonsterAction::increment(const int by) {
    const auto c = character();
    const auto at = c->actionType();
    if(mStage == eMonsterAttackStage::attacking &&
       mAttackActionType == eCharacterActionType::fight) {
        if(incrementMeleeAttack(by)) return;
    }
    if(at == eCharacterActionType::walk &&
       (mStage == eMonsterAttackStage::goTo ||
        mStage == eMonsterAttackStage::patrol)) {
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

    GodMonsterAction::increment(by);
}

bool MonsterAction::decide() {
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

void MonsterAction::beginAttack(const eMissileTarget& target,
                                 const eCharacterActionType at,
                                 const eMonsterAttackStage prevStage) {
    mPreAttackStage = prevStage;
    mAttackTarget = target;
    mAttackActionType = at;
    mAttackTime = eMonster::sMonsterAttackTime(mType);
    mStage = eMonsterAttackStage::attacking;
    mLookForAttack = 0;
}

void MonsterAction::finishAttack() {
    mStage = mPreAttackStage;
    mPreAttackStage = eMonsterAttackStage::none;
    mAttackTarget = eMissileTarget();
    mAttackTime = 0;
}

void MonsterAction::retaliate(eCharacter* const attacker) {
    if(!attacker || attacker->dead()) return;
    const auto c = character();
    if(!c || c->dead()) return;
    // already busy attacking something, leave it be
    if(mStage == eMonsterAttackStage::attacking ||
       mStage == eMonsterAttackStage::destroyingBuilding) return;
    const auto ct = c->tile();
    const auto at = attacker->tile();
    if(!ct || !at) return;

    const int dx = std::abs(ct->x() - at->x());
    const int dy = std::abs(ct->y() - at->y());
    const int dist = std::max(dx, dy);

    const eMissileTarget target(attacker);
    pauseAction();
    if(dist <= 1) {
        beginAttack(target, eCharacterActionType::fight, mStage);
        spawnMeleeAttack();
    } else {
        beginAttack(target, eCharacterActionType::fight2, mStage);
        spawnAttackMissile();
    }
}

void MonsterAction::spawnAttackMissile() {
    const auto c = character();
    const auto chart = c->type();
    const auto act = std::make_shared<eLookForAttackGodAct>(board(), c);
    if(const auto target = mAttackTarget.target()) act->find(target);
    const auto finishAttackA = std::make_shared<eMA_lookForRangeActionFinishAttack>(
                                   board(), this);
    spawnMissile(mAttackActionType, chart, mAttackTime,
                 mAttackTarget, nullptr, act, finishAttackA);
}

void MonsterAction::spawnMeleeAttack() {
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

bool MonsterAction::incrementMeleeAttack(const int by) {
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

void MonsterAction::spawnBuildingAttackMissiles() {
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

void MonsterAction::serializeFields(eSaveArchive& ar) {
    GodMonsterAction::serializeFields(ar);
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

void MonsterAction::resumeFromSavedState() {
    rebuildCurrentStage();
}

void MonsterAction::rebuildCurrentStage() {
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
        else GodMonsterAction::resumeFromSavedState();
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
        GodMonsterAction::resumeFromSavedState();
        return;
    }
}

eTile* MonsterAction::closestEmptySpace(const int rdx, const int rdy) const {
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

void MonsterAction::randomPlaceOnBoard() {
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
//    const stdptr<MonsterAction> tptr(this);
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

stdsptr<eObsticleHandler> MonsterAction::obsticleHandler() {
    return std::make_shared<eMonsterObsticleHandler>(board(), this);
}

void MonsterAction::goToTarget() {
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
        const stdptr<MonsterAction> tptr(this);
        a->setFoundAction([tptr, this]() {
            if(!tptr) return;
            const auto c = character();
            c->setActionType(eCharacterActionType::walk);
        });
        a->setRemoveLastTurn(true);

        a->start(underBuilding, WalkableObject::sCreateDeepWater());
        setCurrentAction(a);
    } else {
        const stdptr<MonsterAction> tptr(this);
        const auto tryAgain = std::make_shared<eGoToTargetTryAgain>(
                                  board(), this);
        GodMonsterAction::goToTarget(eHeatGetters::any, tryAgain,
                                      obsticleHandler(),
                                      eWalkableHelpers::sMonsterTileDistance,
                                      WalkableObject::sCreateAttacker(),
                                      WalkableObject::sCreateDefault());
    }
}

void MonsterAction::goBack() {
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
        a->start(mHomeTile, WalkableObject::sCreateDeepWater());
    } else {
        a->start(mHomeTile, WalkableObject::sCreateAttacker(),
                 WalkableObject::sCreateDefault());
    }
    setCurrentAction(a);
    c->setActionType(eCharacterActionType::walk);
}

void MonsterAction::monsterPatrol() {
    enterMonsterPatrol();
}

void MonsterAction::enterMonsterPatrol() {
    if(mType == eMonsterType::scylla ||
       mType == eMonsterType::kraken) {
        mPatrolRemaining = moveAroundPeriod();
        rebuildMonsterPatrol();
    } else {
        mPatrolRemaining = 0;
        goToNearestRoad();
    }
}

void MonsterAction::rebuildMonsterPatrol() {
    if(mType == eMonsterType::scylla ||
       mType == eMonsterType::kraken) {
        moveAround(nullptr, mPatrolRemaining, WalkableObject::sCreateDeepWater());
    } else {
        goToNearestRoad();
    }
}

void MonsterAction::destroyBuilding(eBuilding* const b) {
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

void MonsterAction::beginBuildingAttack(eBuilding* const b,
                                         const eMonsterAttackStage prevStage) {
    mPreAttackStage = prevStage;
    mAttackBuilding = b;
    mStage = eMonsterAttackStage::destroyingBuilding;
    mLookForAttack = 0;
}

void MonsterAction::finishBuildingAttack() {
    mStage = mPreAttackStage;
    mPreAttackStage = eMonsterAttackStage::none;
    mAttackBuilding = nullptr;
}

void MonsterAction::enterWait() {
    mWaitRemaining = invadePeriod();
    rebuildWait();
}

void MonsterAction::rebuildWait() {
    if(mType == eMonsterType::scylla ||
       mType == eMonsterType::kraken) {
        moveAround(nullptr, mWaitRemaining,
                   WalkableObject::sCreateDeepWater());
    } else {
        moveAround(nullptr, mWaitRemaining);
    }
}

bool MonsterAction::lookForAttack(const int dtime,
                                   int& time, const int freq,
                                   const int range) {
    const auto c = character();
    const auto act = std::make_shared<eLookForAttackGodAct>(
                         board(), c);

    const auto at = eCharacterActionType::fight2;

    return lookForRangeAction(dtime, time, freq, range,
                              at, act, nullptr);
}

bool MonsterAction::lookForMeleeAttack(const bool charactersOnly,
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

bool MonsterAction::lookForAnyAttack(const int dtime,
                                      int& time, const int freq,
                                      const int range) {
    time += dtime;
    if(time <= freq) return false;
    time -= freq;
    const auto rangedBuilding = [this, &time, range]() {
        return lookForRangeAction(
            0, time, 0, range, eCharacterActionType::fight2,
            std::make_shared<eLookForAttackGodAct>(board(), character()),
            nullptr, true);
    };
    const int roll = eRand::rand() % 4;
    if(roll < 2) {
        if(rangedBuilding()) return true;
        if(lookForMeleeAttack(false, true)) return true;
        return lookForMeleeAttack(true);
    } else if(roll == 2) {
        if(lookForMeleeAttack(true)) return true;
        if(lookForMeleeAttack(false, true)) return true;
        return rangedBuilding();
    } else {
        if(lookForMeleeAttack(false, true)) return true;
        if(rangedBuilding()) return true;
        return lookForMeleeAttack(true);
    }
}

bool MonsterAction::lookForRangeAction(const int dtime,
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

int MonsterAction::attackPeriod() const {
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

int MonsterAction::invadePeriod() const {
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

int MonsterAction::moveAroundPeriod() const {
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
