#ifndef ESHEPHERDACTION_H
#define ESHEPHERDACTION_H

#include "eactionwithcomeback.h"

#include "characters/shepherd.h"
#include "buildings/eshepherbuildingbase.h"
#include "characters/edomesticatedanimal.h"

class eDomesticatedAnimal;
class eMoveToAction;
class eSaveArchive;

enum class eShepherdActionStage {
    idle, findingAnimal, collecting, grooming, goingBack, waiting
};

class eShepherdAction : public eActionWithComeback {
    friend class eSA_collectDecisionFinish;
    friend class eSA_groomDecisionFinish;
public:
    eShepherdAction(eShepherBuildingBase* const shed,
                    eResourceCollectorBase* const c,
                    const eCharacterType animalType);
    eShepherdAction(eCharacter* const c);
    ~eShepherdAction();

    bool decide() override;
    void increment(const int by) override;
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void rebuildCurrentStage();
    void rebuildFindAnimal();
    bool findResourceDecision();
    stdsptr<eMoveToAction> makeFindAnimalMove();
    eDomesticatedAnimal* findAnimal(bool wantShearable);
    void reserveAnimal(eDomesticatedAnimal* a);
    void releaseAnimal(eDomesticatedAnimal* a);
    void collectDecision(eDomesticatedAnimal* const a);
    void groomDecision(eDomesticatedAnimal* const a);
    void goBackDecision();
    void waitDecision();

    eCharacterType mAnimalType;

    eResourceCollectorBase* mCharacter = nullptr;
    stdptr<eShepherBuildingBase> mShed;

    bool mFinishOnce = false;
    int mGroomed = 0;
    bool mNoResource = false;
    eShepherdActionStage mStage = eShepherdActionStage::idle;
    int mWaitRemaining = 0;
    stdptr<eDomesticatedAnimal> mLastAnimal;
    stdptr<eDomesticatedAnimal> mTargetAnimal;
};

class eSA_collectDecisionFinish : public eCharActFunc {
public:
    eSA_collectDecisionFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::SA_collectDecisionFinish) {}
    eSA_collectDecisionFinish(GameBoard& board, eShepherdAction* const t,
                              eDomesticatedAnimal* const a) :
        eCharActFunc(board, eCharActFuncType::SA_collectDecisionFinish),
        mTptr(t), mAptr(a) {}

    void call() override {
        int c = 0;
        if(mAptr) {
            const auto a = mAptr.get();
            c = a->collect();
            a->setBusy(false);
            a->setVisible(true);
            a->resumeAction();
        }
        if(!mTptr) return;
        mTptr->mCharacter->incCollected(c);
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
        ar.characterAsField("animal", &board(), mAptr);
    }
private:
    stdptr<eShepherdAction> mTptr;
    stdptr<eDomesticatedAnimal> mAptr;
};

class eSA_collectDecisionDeleteFail : public eCharActFunc {
public:
    eSA_collectDecisionDeleteFail(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::SA_collectDecisionDeleteFail) {}
    eSA_collectDecisionDeleteFail(GameBoard& board, eDomesticatedAnimal* const a) :
        eCharActFunc(board, eCharActFuncType::SA_collectDecisionDeleteFail),
        mAptr(a) {}

    void call() override {
        if(!mAptr) return;
        const auto a = mAptr.get();
        a->setBusy(false);
        a->setVisible(true);
        a->resumeAction();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterAsField("animal", &board(), mAptr);
    }
private:
    stdptr<eDomesticatedAnimal> mAptr;
};

class eSA_groomDecisionFinish : public eCharActFunc {
public:
    eSA_groomDecisionFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::SA_groomDecisionFinish) {}
    eSA_groomDecisionFinish(GameBoard& board, eShepherdAction* const t,
                            eDomesticatedAnimal* const a) :
        eCharActFunc(board, eCharActFuncType::SA_groomDecisionFinish),
        mTptr(t), mAptr(a) {}

    void call() override {
        if(mAptr) {
            const auto a = mAptr.get();
            a->groom();
            a->setBusy(false);
            a->resumeAction();
        }
        if(!mTptr) return;
        mTptr->mGroomed++;
        mTptr->goBackDecision();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
        ar.characterAsField("animal", &board(), mAptr);
    }
private:
    stdptr<eShepherdAction> mTptr;
    stdptr<eDomesticatedAnimal> mAptr;
};

class eSA_groomDecisionDeleteFail : public eCharActFunc {
public:
    eSA_groomDecisionDeleteFail(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::SA_groomDecisionDeleteFail) {}
    eSA_groomDecisionDeleteFail(GameBoard& board, eDomesticatedAnimal* const a) :
        eCharActFunc(board, eCharActFuncType::SA_groomDecisionDeleteFail),
        mAptr(a) {}

    void call() override {
        if(!mAptr) return;
        const auto a = mAptr.get();
        a->setBusy(false);
        a->resumeAction();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterAsField("animal", &board(), mAptr);
    }
private:
    stdptr<eDomesticatedAnimal> mAptr;
};

#endif // ESHEPHERDACTION_H
