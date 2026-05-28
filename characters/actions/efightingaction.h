#ifndef EFIGHTINGACTION_H
#define EFIGHTINGACTION_H

#include "ecomplexaction.h"

#include "characters/echaracter.h"
#include "walkable/eobsticlehandler.h"

class eSaveArchive;

class eAttackTarget {
public:
    eAttackTarget();
    eAttackTarget(eCharacter* const c);
    eAttackTarget(eBuilding* const b);

    eTile* tile() const;
    bool valid() const;
    bool takeDamage(const double a, eCharacter* const attacker = nullptr);
    bool takeMeleeDamage(const double a, eCharacter* const attacker = nullptr);
    bool dead() const;
    void clear();

    int armor() const;
    int armorVsMissiles() const;

    bool building() const;
    eCharacter* character() const { return mC.get(); }

    double absX() const;
    double absY() const;

    void serialize(eSaveArchive& ar, GameBoard& board);
private:
    stdptr<eCharacter> mC;
    stdptr<eBuilding> mB;
};

enum class eLookForEnemyState {
    dead, attacking, none
};

enum class eFightingSavedMove {
    none, goTo, waitGoHome
};

class eFightingAction : public eComplexAction {
    friend class eSoldierObsticleHandler;
public:
    using eComplexAction::eComplexAction;

    static void sSignalBeingAttack(eCharacter* const attacked,
                                   eCharacter* const by,
                                   GameBoard& brd);
    static void sSignalBeingAttack(eCharacter* const attacked,
                                   const int ttx, const int tty,
                                   GameBoard& brd);

    eLookForEnemyState lookForEnemy(const int by);

    using eAction = std::function<void()>;
    void goTo(const int fx, const int fy,
              const int dist = 0,
              const eAction& findFailAct = nullptr,
              const eAction& findFinishAct = nullptr);

    void beingAttacked(eCharacter* const ss);
    void beingAttacked(const int ttx, const int tty);

    void waitAndGoHome(const int w);
    virtual void goHome() = 0;
    virtual void goAbroad() = 0;

    void setOverwrittableAction(const bool o)
    { mOverwrittableAction = o; }
protected:
    bool isAttacking() const { return mAttack; }
    void cancelAttack();
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    virtual stdsptr<eObsticleHandler> obsticleHandler() { return nullptr; }
    bool attackBuilding(eTile* const t, const bool range);
    void rebuildSavedRuntime();
    bool atSavedMoveTarget() const;

    double mAngle{0.};

    int mMissile = 0;
    int mMeleeTime = 0;

    int mRangeAttack = 0;
    int mBuildingAttack = 0;

    int mLookForEnemy = 0;
    int mAttackTime = 0;
    eCharacterActionType mSavedAction = eCharacterActionType::stand;
    bool mAttack = false;
    bool mAttackRanged = false;
    bool mOverwrittableAction = false;
    eAttackTarget mAttackTarget;
    eFightingSavedMove mSavedMove = eFightingSavedMove::none;
    int mSavedMoveX = 0;
    int mSavedMoveY = 0;
    int mSavedMoveDistance = 0;
    int mWaitGoHomeRemaining = 0;
};

class eSA_goToFinish : public eCharActFunc {
public:
    eSA_goToFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::SA_goToFinish) {}
    eSA_goToFinish(GameBoard& board, eCharacter* const c) :
        eCharActFunc(board, eCharActFuncType::SA_goToFinish),
        mCptr(c) {}

    void call() override {
        if(!mCptr) return;
        mCptr->setActionType(eCharacterActionType::stand);
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterField("character", &board(), mCptr);
    }
private:
    stdptr<eCharacter> mCptr;
};

class eSA_waitAndGoHomeFinish : public eCharActFunc {
public:
    eSA_waitAndGoHomeFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::SA_waitAndGoHomeFinish) {}
    eSA_waitAndGoHomeFinish(GameBoard& board, eFightingAction* const a) :
        eCharActFunc(board, eCharActFuncType::SA_waitAndGoHomeFinish),
        mAptr(a) {}

    void call() override {
        if(!mAptr) return;
        mAptr->goHome();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mAptr);
    }
private:
    stdptr<eFightingAction> mAptr;
};

#endif // EFIGHTINGACTION_H
