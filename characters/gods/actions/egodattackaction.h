#ifndef EGODATTACKACTION_H
#define EGODATTACKACTION_H

#include "characters/gods/actions/god-action.h"
#include "elanguage.h"
#include "characters/actions/walkable/eobsticlehandler.h"
#include "etilehelper.h"

class eSaveArchive;

enum class eGodAttackStage {
    none, appear, goTo1, patrol1, goTo2, patrol2, disappear,
    attacking, destroyingBuilding
};

enum class eGodSound;

class eGodAttackAction : public eGodAction {
    friend class eGodObsticleHandler;
public:
    eGodAttackAction(eCharacter* const c);
    ~eGodAttackAction();

    void increment(const int by) override;
    bool decide() override;

    void setSanctuary(const stdptr<eSanctuary>& s);

    void rebuildCurrentStage();
    void finishAttacking();
    void finishBuildingAttack();
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void initialize();

    bool lookForAttack(const int dtime, int& time,
                       const int freq, const int range);
    bool lookForTargetedAttack(const int dtime, int& time,
                               const int freq, const int range);

    bool lookForRangeAction(const int dtime, int& time,
                            const int freq, const int range,
                            const eCharacterActionType at,
                            const stdsptr<eGodAct>& act,
                            const eCharacterType chart,
                            const eGodSound missileSound,
                            const int nMissiles = 1) override;

    void goToTarget();
    stdsptr<eObsticleHandler> obsticleHandler();
    void destroyBuilding(eBuilding* const b);

    void beginAttacking(const eMissileTarget& target,
                        const eGodActType kind,
                        const eCharacterActionType at,
                        const eGodSound sound,
                        const int nMissiles,
                        const double bless,
                        const eGodAttackStage prevStage);
    void spawnAttackMissile();
    void spawnDestroyBuildingMissile(eBuilding* const b);
    stdsptr<eGodAct> rebuildAttackAct();

    eGodAttackStage mStage{eGodAttackStage::none};

    int mLookForCurse = eRand::rand() % 2000;
    int mLookForTargetedCurse = eRand::rand() % 2000;
    int mLookForAttack = eRand::rand() % 2000;
    int mLookForTargetedAttack = eRand::rand() % 2000;
    int mLookForGod = eRand::rand() % 2000;
    int mLookForSpecial = eRand::rand() % 2000;

    stdptr<eSanctuary> mSanctuary;

    eGodAttackStage mPreAttackStage{eGodAttackStage::none};
    eGodActType mAttackKind{eGodActType::lookForAttack};
    eMissileTarget mAttackTarget;
    eCharacterActionType mAttackActionType{eCharacterActionType::fight2};
    int mAttackSoundInt = 0;
    int mAttackNMissiles = 1;
    double mAttackBless = 0.0;

    stdptr<eBuilding> mAttackBuilding;
};

class eGAA_loserDisappearFinish : public eCharActFunc {
public:
    eGAA_loserDisappearFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GAA_loserDisappearFinish) {}
    eGAA_loserDisappearFinish(eGameBoard& board, eGodMonsterAction* const ca) :
        eCharActFunc(board, eCharActFuncType::GAA_loserDisappearFinish),
        mLoserPtr(ca) {}

    void call() override {
        if(!mLoserPtr) return;
        const auto t = mLoserPtr.get();
        const auto c = t->character();
        c->kill();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("loser", &board(), mLoserPtr);
    }
private:
    stdptr<eGodMonsterAction> mLoserPtr;
};


class eGAA_destroyBuildingFinish : public eCharActFunc {
public:
    eGAA_destroyBuildingFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GAA_destroyBuildingFinish) {}
    eGAA_destroyBuildingFinish(eGameBoard& board,
                               eGodAttackAction* const tptr,
                               eBuilding* const b) :
        eCharActFunc(board, eCharActFuncType::GAA_destroyBuildingFinish),
        mTptr(tptr), mBptr(b) {}

    void call() override {
        const stdptr<eGodAttackAction> t = mTptr;
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
    stdptr<eGodAttackAction> mTptr;
    stdptr<eBuilding> mBptr;
};

class eGAA_rangeAttackFinish : public eCharActFunc {
public:
    eGAA_rangeAttackFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GAA_rangeAttackFinish) {}
    eGAA_rangeAttackFinish(eGameBoard& board, eGodAttackAction* const ca) :
        eCharActFunc(board, eCharActFuncType::GAA_rangeAttackFinish),
        mTptr(ca) {}

    void call() override {
        const stdptr<eGodAttackAction> t = mTptr;
        if(!t) return;
        t->finishAttacking();
        t->resumeAction();
        if(t && !t->currentAction()) t->rebuildCurrentStage();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eGodAttackAction> mTptr;
};

class eTeleportFindFailFunc : public eFindFailFunc {
public:
    eTeleportFindFailFunc(eGameBoard& board) :
        eFindFailFunc(board, eFindFailFuncType::teleport) {}
    eTeleportFindFailFunc(eGameBoard& board, eGodAction* const ca) :
        eFindFailFunc(board, eFindFailFuncType::teleport),
        mTptr(ca) {}

    void call(eTile* const tile) override {
        if(!mTptr) return;
        const auto c = mTptr->character();
        auto& board = c->getBoard();
        const auto r = eTileHelper::closestRoad(tile->x(), tile->y(), board);
        mTptr->teleport(r);
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eGodAction> mTptr;
};

class eGodObsticleHandler : public eObsticleHandler {
public:
    eGodObsticleHandler(eGameBoard& board) :
        eObsticleHandler(board, eObsticleHandlerType::god) {}
    eGodObsticleHandler(eGameBoard& board,
                        eGodAttackAction* const t) :
        eObsticleHandler(board, eObsticleHandlerType::god),
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
    stdptr<eGodAttackAction> mTptr;
};

#endif // EGODATTACKACTION_H
