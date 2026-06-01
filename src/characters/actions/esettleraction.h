#ifndef ESETTLERACTION_H
#define ESETTLERACTION_H

#include "eactionwithcomeback.h"

class eSaveArchive;

enum class eSettlerActionStage {
    idle, findingHouse, goingBack, leaving
};

class eSettlerAction : public eActionWithComeback {
    friend class eSA_findHouseFail;
    friend class eSA_findHouseFinish;
public:
    eSettlerAction(eCharacter* const c);

    bool decide() override;

    void setNumberPeople(const int p);
    void setInitialWait(const int w);
    int nPeople() const { return mNPeople; }
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
    void findHouse();
    void goBack2();
    void leave();
    bool enterHouse();

    int mNPeople = 0;
    int mInitialWait = 0;
    bool mNoHouses = false;
    bool mEliteOnly = true;
    eSettlerActionStage mStage = eSettlerActionStage::idle;
};

class eSA_findHouseFail : public eCharActFunc {
public:
    eSA_findHouseFail(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::SA_findHouseFail) {}
    eSA_findHouseFail(GameBoard& board, eSettlerAction* const t) :
        eCharActFunc(board, eCharActFuncType::SA_findHouseFail),
        mTptr(t) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        t->findHouse();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eSettlerAction> mTptr;
};

class eSA_findHouseFinish : public eCharActFunc {
public:
    eSA_findHouseFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::SA_findHouseFinish) {}
    eSA_findHouseFinish(GameBoard& board, eSettlerAction* const t) :
        eCharActFunc(board, eCharActFuncType::SA_findHouseFinish),
        mTptr(t) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        const bool r = t->enterHouse();
        const int nPeople = t->nPeople();
        if(r && nPeople <= 0) {
            t->setState(eCharacterActionState::finished);
        }
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eSettlerAction> mTptr;
};

#endif // ESETTLERACTION_H
