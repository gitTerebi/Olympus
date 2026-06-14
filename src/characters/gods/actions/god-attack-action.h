#ifndef EGODATTACKACTION_H
#define EGODATTACKACTION_H

#include "characters/gods/actions/god-action.h"
#include "elanguage.h"
#include "characters/actions/walkable/eobsticlehandler.h"
#include "etilehelper.h"

class eSaveArchive;

enum class GodAttackStage {
    none, appear, goTo1, patrol1, goTo2, patrol2, disappear,
    attacking, destroyingBuilding
};

enum class eGodSound;

class GodAttackAction : public eGodAction {
    friend class eGodObsticleHandler;
public:
    GodAttackAction(eCharacter* const c);
    ~GodAttackAction();

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
                        const GodAttackStage prevStage);
    void spawnAttackMissile();
    void spawnDestroyBuildingMissile(eBuilding* const b);
    stdsptr<eGodAct> rebuildAttackAct();

    GodAttackStage mStage{GodAttackStage::none};

    int mLookForCurse = eRand::rand() % 2000;
    int mLookForTargetedCurse = eRand::rand() % 2000;
    int mLookForAttack = eRand::rand() % 2000;
    int mLookForTargetedAttack = eRand::rand() % 2000;
    int mLookForGod = eRand::rand() % 2000;
    int mLookForSpecial = eRand::rand() % 2000;

    stdptr<eSanctuary> mSanctuary;

    GodAttackStage mPreAttackStage{GodAttackStage::none};
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
    eGAA_loserDisappearFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GAA_loserDisappearFinish) {}
    eGAA_loserDisappearFinish(GameBoard& board, GodMonsterAction* const ca) :
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
    stdptr<GodMonsterAction> mLoserPtr;
};


class eGAA_destroyBuildingFinish : public eCharActFunc {
public:
    eGAA_destroyBuildingFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GAA_destroyBuildingFinish) {}
    eGAA_destroyBuildingFinish(GameBoard& board,
                               GodAttackAction* const tptr,
                               eBuilding* const b) :
        eCharActFunc(board, eCharActFuncType::GAA_destroyBuildingFinish),
        mTptr(tptr), mBptr(b) {}

    void call() override {
        const stdptr<GodAttackAction> t = mTptr;
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
    stdptr<GodAttackAction> mTptr;
    stdptr<eBuilding> mBptr;
};

class eGAA_rangeAttackFinish : public eCharActFunc {
public:
    eGAA_rangeAttackFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GAA_rangeAttackFinish) {}
    eGAA_rangeAttackFinish(GameBoard& board, GodAttackAction* const ca) :
        eCharActFunc(board, eCharActFuncType::GAA_rangeAttackFinish),
        mTptr(ca) {}

    void call() override {
        const stdptr<GodAttackAction> t = mTptr;
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
    stdptr<GodAttackAction> mTptr;
};

class eTeleportFindFailFunc : public eFindFailFunc {
public:
    eTeleportFindFailFunc(GameBoard& board) :
        eFindFailFunc(board, eFindFailFuncType::teleport) {}
    eTeleportFindFailFunc(GameBoard& board, eGodAction* const ca) :
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
    eGodObsticleHandler(GameBoard& board) :
        eObsticleHandler(board, eObsticleHandlerType::god) {}
    eGodObsticleHandler(GameBoard& board,
                        GodAttackAction* const t) :
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
    stdptr<GodAttackAction> mTptr;
};

#endif // EGODATTACKACTION_H
