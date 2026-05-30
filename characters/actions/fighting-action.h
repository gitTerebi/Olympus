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
    ~FightingAction();

    static void sSignalBeingAttack(eCharacter* const attacked,
                                   eCharacter* const by,
                                   GameBoard& brd);
    static void sSignalBeingAttack(eCharacter* const attacked,
                                   const int ttx, const int tty,
                                   GameBoard& brd);

    LookForEnemyState lookForEnemy(const int by);

    // How far a unit notices an enemy worth engaging. Ranged: firing range plus
    // a wide-awareness margin; it detects this far but only steps a few tiles to
    // a firing position, then holds. Melee: a short box — the banner march
    // already drops them next to the enemy, this just lets each soldier close
    // the last couple tiles onto its own nearest free target (Augustus
    // enemy_fighting per-figure approach) so back ranks engage instead of
    // standing in slots while only the front two fight.
    static int sRangedDetectRange(const int range)
    { return range > 0 ? range + 2 : 4; }

    using eAction = std::function<void()>;
    void goTo(const int fx, const int fy,
              const int dist = 0,
              const eAction& findFailAct = nullptr,
              const eAction& findFinishAct = nullptr);

    void beingAttacked(eCharacter* const ss);
    virtual void beingAttacked(int ttx, int tty);

    void waitAndGoHome(const int w);
    virtual void goHome() = 0;
    virtual void goAbroad() = 0;

    void setOverwrittableAction(const bool o)
    { mOverwrittableAction = o; }
    bool overwrittableAction() const { return mOverwrittableAction; }
protected:
    bool isAttacking() const { return mAttack; }
    void cancelAttack();

    // Claim/release a melee retaliation target so attackers spread across the
    // enemy line instead of dogpiling one foe (Augustus targeted_by model).
    // claimTarget releases any prior claim first. releaseClaim is safe to call
    // when nothing is claimed.
    void claimTarget(eCharacter* const c);
    void releaseClaim();
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;

    // Anchor tile a ranged unit fires from. It steps at most a couple tiles
    // from here to bring an enemy into range, then holds — it never walks to
    // the enemy tile. Default: the unit's current tile. Soldiers override to
    // return their formation slot so they hold the banner line.
    virtual eTile* repositionAnchor() const;
    bool attackBuilding(eTile* const t, const bool range);
private:
    virtual stdsptr<eObsticleHandler> obsticleHandler() { return nullptr; }
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
    stdptr<eCharacter> mClaimedTarget; // runtime-only; see claimTarget/releaseClaim
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
