#ifndef EARCHERACTION_H
#define EARCHERACTION_H

#include "ecomplexaction.h"
#include "pointers/estdpointer.h"

class eSaveArchive;

class eArcherAction : public eComplexAction {
public:
    eArcherAction(eCharacter* const c);

    void increment(const int by);
    bool decide();
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:

    int mMissile = 0;
    int mRangeAttack = 0;
    int mAttackTime = 0;
    bool mAttack = false;
    stdptr<eCharacter> mAttackTarget;
};

class eAA_patrolFail : public eCharActFunc {
public:
    eAA_patrolFail(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::AA_patrolFail) {}
    eAA_patrolFail(eGameBoard& board, eCharacterAction* const t) :
        eCharActFunc(board, eCharActFuncType::AA_patrolFail),
        mTptr(t) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        t->setState(eCharacterActionState::failed);
    }
protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eCharacterAction> mTptr;
};

class eAA_patrolFinish : public eCharActFunc {
public:
    eAA_patrolFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::AA_patrolFinish) {}
    eAA_patrolFinish(eGameBoard& board, eCharacterAction* const t) :
        eCharActFunc(board, eCharActFuncType::AA_patrolFinish),
        mTptr(t) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        t->setState(eCharacterActionState::finished);
    }
protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eCharacterAction> mTptr;
};

#endif // EARCHERACTION_H
