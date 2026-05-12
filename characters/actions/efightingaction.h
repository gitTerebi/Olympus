#ifndef EFIGHTINGACTION_H
#define EFIGHTINGACTION_H

#include "ecomplexaction.h"

#include "characters/echaracter.h"
#include "fileIO/ejsonarchive.h"

class eSaveArchive;

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
    void serializeJson(const char* key, eJsonArchive& ar, eGameBoard& board);
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
    void serializeJson(eJsonArchive& ar) override;

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
private:
    void serialize(eSaveArchive& ar);

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

    void serializeJson(eJsonArchive& ar) override {
        if(ar.writing()) {
            int ioid = mCptr ? mCptr->ioID() : -1;
            ar.field("mCptr", ioid);
        } else {
            int ioid = -1;
            ar.field("mCptr", ioid);
            if(ioid >= 0) {
                ar.addPostFunc([this, ioid]() {
                    mCptr = resolveChar(ioid);
                });
            }
        }
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

    void serializeJson(eJsonArchive& ar) override {
        if(ar.writing()) {
            int ioid = mAptr ? mAptr->ioID() : -1;
            ar.field("mAptr", ioid);
        } else {
            int ioid = -1;
            ar.field("mAptr", ioid);
            if(ioid >= 0) {
                ar.addPostFunc([this, ioid]() {
                    mAptr = static_cast<eFightingAction*>(resolveCharAction(ioid));
                });
            }
        }
    }
private:
    stdptr<eFightingAction> mAptr;
};

#endif // EFIGHTINGACTION_H
