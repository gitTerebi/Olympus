#ifndef FIGHTING_ACTION_H
#define FIGHTING_ACTION_H

#include "ecomplexaction.h"

#include "characters/echaracter.h"
#include "walkable/eobsticlehandler.h"

class eSaveArchive;

class AttackTarget {
public:
    AttackTarget();
    AttackTarget(eCharacter* const c);
    AttackTarget(eBuilding* const b);

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

enum class LookForEnemyState {
    dead, attacking, none
};

enum class FightingSavedMove {
    none, goTo, waitGoHome
};

class FightingAction : public eComplexAction {
    friend class SoldierObsticleHandler;
public:
    using eComplexAction::eComplexAction;

    static void sSignalBeingAttack(eCharacter* const attacked,
                                   eCharacter* const by,
                                   GameBoard& brd);
    static void sSignalBeingAttack(eCharacter* const attacked,
                                   const int ttx, const int tty,
                                   GameBoard& brd);

    LookForEnemyState lookForEnemy(const int by);

    // How far a ranged unit notices an enemy worth engaging: its firing range
    // plus a wide-awareness margin. The unit detects this far but only steps a
    // few tiles toward a firing position; it holds and waits rather than
    // chasing. Used by both the reposition scan and the banner-return gate so
    // the two never drift. range 0 (melee) keeps the old tight 3-tile box.
    static int sRangedDetectRange(const int range)
    { return range > 0 ? range + 8 : 3; }

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

    // Anchor tile a ranged unit fires from. It steps at most a couple tiles
    // from here to bring an enemy into range, then holds — it never walks to
    // the enemy tile. Default: the unit's current tile. Soldiers override to
    // return their formation slot so they hold the banner line.
    virtual eTile* repositionAnchor() const;
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
    AttackTarget mAttackTarget;
    FightingSavedMove mSavedMove = FightingSavedMove::none;
    int mSavedMoveX = 0;
    int mSavedMoveY = 0;
    int mSavedMoveDistance = 0;
    int mWaitGoHomeRemaining = 0;
};

class SA_goToFinish : public eCharActFunc {
public:
    SA_goToFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::SA_goToFinish) {}
    SA_goToFinish(GameBoard& board, eCharacter* const c) :
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

class SA_waitAndGoHomeFinish : public eCharActFunc {
public:
    SA_waitAndGoHomeFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::SA_waitAndGoHomeFinish) {}
    SA_waitAndGoHomeFinish(GameBoard& board, FightingAction* const a) :
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
    stdptr<FightingAction> mAptr;
};

#endif // FIGHTING_ACTION_H
