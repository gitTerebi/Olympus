#ifndef EFIGHTINGACTION_H
#define EFIGHTINGACTION_H

#include "ecomplexaction.h"

#include "characters/echaracter.h"

class eAttackTarget {
public:
    eAttackTarget();
    eAttackTarget(eCharacter* const c);
    eAttackTarget(eBuilding* const b);

    eTile* tile() const;
    bool valid() const;
    bool defend(const double a);
    bool dead() const;
    void clear();

    bool building() const;

    double absX() const;
    double absY() const;

    void read(eGameBoard& board, eReadStream& src);
    void write(eWriteStream& dst) const;
private:
    stdptr<eCharacter> mC;
    stdptr<eBuilding> mB;
};

enum class eLookForEnemyState {
    dead, attacking, none
};

class eFightingAction : public eComplexAction {
    friend class eSoldierObsticleHandler;
public:
    using eComplexAction::eComplexAction;

    static void sSignalBeingAttack(eCharacter* const attacked,
                                   eCharacter* const by,
                                   eGameBoard& brd);
    static void sSignalBeingAttack(eCharacter* const attacked,
                                   const int ttx, const int tty,
                                   eGameBoard& brd);

    eLookForEnemyState lookForEnemy(const int by);

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

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
protected:
    bool isAttacking() const { return mAttack; }

    void setOverwrittableAction(const bool o)
    { mOverwrittableAction = o; }
private:
    virtual stdsptr<eObsticleHandler> obsticleHandler() { return nullptr; }
    bool attackBuilding(eTile* const t, const bool range);

    double mAngle{0.};

    int mMissile = 0;

    int mRangeAttack = 0;
    int mBuildingAttack = 0;

    int mLookForEnemy = 0;
    int mAttackTime = 0;
    eCharacterActionType mSavedAction = eCharacterActionType::stand;
    bool mAttack = false;
    bool mOverwrittableAction = false;
    eAttackTarget mAttackTarget;
};

class eSA_goToFinish : public eCharActFunc {
public:
    eSA_goToFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::SA_goToFinish) {}
    eSA_goToFinish(eGameBoard& board, eCharacter* const c) :
        eCharActFunc(board, eCharActFuncType::SA_goToFinish),
        mCptr(c) {}

    void call() override {
        if(!mCptr) return;
        mCptr->setActionType(eCharacterActionType::stand);
    }

    void read(eReadStream& src) override {
        src.readCharacter(&board(), [this](eCharacter* const c) {
            mCptr = c;
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacter(mCptr);
    }
private:
    stdptr<eCharacter> mCptr;
};

class eSA_waitAndGoHomeFinish : public eCharActFunc {
public:
    eSA_waitAndGoHomeFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::SA_waitAndGoHomeFinish) {}
    eSA_waitAndGoHomeFinish(eGameBoard& board, eFightingAction* const a) :
        eCharActFunc(board, eCharActFuncType::SA_waitAndGoHomeFinish),
        mAptr(a) {}

    void call() override {
        if(!mAptr) return;
        mAptr->goHome();
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const a) {
            mAptr = static_cast<eFightingAction*>(a);
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mAptr);
    }
private:
    stdptr<eFightingAction> mAptr;
};

#endif // EFIGHTINGACTION_H
