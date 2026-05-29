#ifndef EMONSTERACTION_H
#define EMONSTERACTION_H

#include "characters/gods/actions/egodmonsteraction.h"

#include "characters/monsters/emonster.h"
#include "audio/sounds.h"
#include "characters/actions/walkable/eobsticlehandler.h"

enum class eMonsterAttackStage {
    none, wait, goTo, patrol, goBack, attacking, destroyingBuilding
};

class eSaveArchive;

class eMonsterAction : public eGodMonsterAction {
    friend class eMonsterObsticleHandler;
public:
    eMonsterAction(eCharacter* const c);

    void increment(const int by) override;
    bool decide() override;

    eMonsterAttackStage stage() const { return mStage; }

    eMonsterAggressivness aggressivness() const
    { return mAggressivness; }
    void setAggressivness(const eMonsterAggressivness a)
    { mAggressivness = a; }

    void goToTarget();
    void goBack();
    void monsterPatrol();

    void beginAttack(const eMissileTarget& target,
                     const eCharacterActionType at,
                     const eMonsterAttackStage prevStage);
    void finishAttack();

    void retaliate(eCharacter* const attacker);

    void beginBuildingAttack(eBuilding* const b,
                             const eMonsterAttackStage prevStage);
    void finishBuildingAttack();

    void rebuildCurrentStage();
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void destroyBuilding(eBuilding* const b);
    bool incrementMeleeAttack(const int by);
    void spawnMeleeAttack();
    void spawnAttackMissile();
    void spawnBuildingAttackMissiles();
    eTile* closestEmptySpace(const int rdx, const int rdy) const;
    void randomPlaceOnBoard();
    stdsptr<eObsticleHandler> obsticleHandler();
    bool lookForAttack(const int dtime, int& time,
                       const int freq, const int range);
    bool lookForMeleeAttack(const bool charactersOnly,
                            const bool buildingsOnly = false);
    bool lookForAnyAttack(const int dtime, int& time,
                          const int freq, const int range);
    bool lookForRangeAction(const int dtime,
                            int& time, const int freq,
                            const int range,
                            const eCharacterActionType at,
                            const stdsptr<eGodAct>& act,
                            const stdsptr<eCharActFunc>& missileSound,
                            const bool buildingsOnly = false);

    int attackPeriod() const;
    int invadePeriod() const;
    int moveAroundPeriod() const;

    eTile* mHomeTile = nullptr;
    eMonsterAggressivness mAggressivness{eMonsterAggressivness::passive};
    eMonsterAttackStage mStage{eMonsterAttackStage::none};
    const eMonsterType mType;
    int mLookForAttack = 0;

    eMonsterAttackStage mPreAttackStage{eMonsterAttackStage::none};
    eMissileTarget mAttackTarget;
    eCharacterActionType mAttackActionType{eCharacterActionType::fight2};
    int mAttackTime = 0;

    stdptr<eBuilding> mAttackBuilding;

    int mWaitRemaining = 0;
    int mPatrolRemaining = 0;

    void enterWait();
    void rebuildWait();
    void enterMonsterPatrol();
    void rebuildMonsterPatrol();
};

class eGoToTargetTryAgain : public eFindFailFunc {
public:
    eGoToTargetTryAgain(GameBoard& board) :
        eFindFailFunc(board, eFindFailFuncType::tryAgain) {}
    eGoToTargetTryAgain(GameBoard& board, eMonsterAction* const ca) :
        eFindFailFunc(board, eFindFailFuncType::tryAgain),
        mTptr(ca) {}

    void call(eTile* const tile) override {
        (void)tile;
        if(!mTptr) return;
        mTptr->setCurrentAction(nullptr);
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eMonsterAction> mTptr;
};

class eMA_destroyBuildingFinish : public eCharActFunc {
public:
    eMA_destroyBuildingFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::MA_destroyBuildingFinish) {}
    eMA_destroyBuildingFinish(GameBoard& board, eMonsterAction* const ca,
                              eBuilding* const b) :
        eCharActFunc(board, eCharActFuncType::MA_destroyBuildingFinish),
        mTptr(ca), mBptr(b) {}

    void call() override {
        const stdptr<eMonsterAction> t = mTptr;
        if(!t) return;
        const auto b = mBptr;
        t->finishBuildingAttack();
        t->resumeAction(); // can delete instance
        if(t && !t->currentAction()) t->rebuildCurrentStage();
        if(!b) return;
        b->collapse();
        eSounds::playCollapseSound();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
        ar.buildingField("building", &board(), mBptr);
    }
private:
    stdptr<eMonsterAction> mTptr;
    stdptr<eBuilding> mBptr;
};

class eMA_lookForRangeActionFinishAttack : public eCharActFunc {
public:
    eMA_lookForRangeActionFinishAttack(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::MA_lookForRangeActionFinishAttack) {}
    eMA_lookForRangeActionFinishAttack(GameBoard& board, eMonsterAction* const ca) :
        eCharActFunc(board, eCharActFuncType::MA_lookForRangeActionFinishAttack),
        mTptr(ca) {}

    void call() override {
        const stdptr<eMonsterAction> t = mTptr;
        if(!t) return;
        t->finishAttack();
        t->resumeAction();
        if(t && !t->currentAction()) t->rebuildCurrentStage();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eMonsterAction> mTptr;
};

class eMonsterObsticleHandler : public eObsticleHandler {
public:
    eMonsterObsticleHandler(GameBoard& board) :
        eObsticleHandler(board, eObsticleHandlerType::monster) {}
    eMonsterObsticleHandler(GameBoard& board,
                            eMonsterAction* const t) :
        eObsticleHandler(board, eObsticleHandlerType::monster),
        mTptr(t) {}

    bool handle(eTile* const tile) override {
        if(!mTptr) return false;
        const auto ub = tile->underBuilding();
        if(!ub) return false;
        const auto ubt = ub->type();
        const bool r = eBuilding::sWalkableBuilding(ubt);
        if(r) return false;
        mTptr->destroyBuilding(ub);
        return true;
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eMonsterAction> mTptr;
};

#endif // EMONSTERACTION_H
