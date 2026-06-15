#ifndef EPROVIDERESOURCEHELPACTION_H
#define EPROVIDERESOURCEHELPACTION_H

#include "characters/gods/actions/god-action.h"
#include "fileIO/save-archive.h"

enum class eProvideResourceHelpStage {
    none, appear, goTo, give, giving, disappear
};

class SaveArchive;

class eProvideResourceHelpAction : public eGodAction {
public:
    eProvideResourceHelpAction(eCharacter* const c,
                               const eCharActionType type,
                               const eResourceType res,
                               const int count);

    bool decide() override;

    void decCount(const int by);

    static bool sHelpNeeded(const eCityId cid,
                            const GameBoard& board,
                            const eResourceType res,
                            const int minSpace);

    void rebuildCurrentStage();
    void finishGiving();
protected:
    void serializeFields(SaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void goToTarget();
    void give();
    void spawnGiveMissile();

    eProvideResourceHelpStage mStage{eProvideResourceHelpStage::none};
    stdptr<eStorageBuilding> mTarget;
    eResourceType mResource = eResourceType::wheat;
    int mCount = 32;
    eProvideResourceHelpStage mPreGivingStage{eProvideResourceHelpStage::none};
};

class eGodProvideResourceAct : public eGodAct {
public:
    eGodProvideResourceAct(GameBoard& board,
                           eProvideResourceHelpAction* const action,
                           eStorageBuilding* const target,
                           const eResourceType resource,
                           const int count) :
        eGodAct(board, eGodActType::provideResource),
        mAction(action),
        mTarget(target),
        mResource(resource),
        mCount(count) {}

    eGodProvideResourceAct(GameBoard& board) :
        eGodProvideResourceAct(board, nullptr, nullptr,
                               eResourceType::meat, 0) {}

    eMissileTarget find(eTile* const t) override {
        (void)t;
        const auto null = static_cast<eTile*>(nullptr);
        return null;
    }

    void act() override {
        if(mTarget) {
            const int added = mTarget->add(mResource, mCount);
            if(mAction) {
                mAction->decCount(added);
            }
        }
    }

    void serializeFields(SaveArchive& ar) override {
        ar.buildingAsField("targetStorage", &board(), mTarget);
        ar.field("resource", mResource);
        ar.field("count", mCount);
        ar.characterActionAsField("sourceAction", &board(), mAction);
    }
private:
    stdptr<eProvideResourceHelpAction> mAction;
    stdptr<eStorageBuilding> mTarget;
    eResourceType mResource;
    int mCount;
};

class ePRHA_giveFinish : public eCharActFunc {
public:
    ePRHA_giveFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::PRHA_giveFinish) {}
    ePRHA_giveFinish(GameBoard& board, eProvideResourceHelpAction* const ca) :
        eCharActFunc(board, eCharActFuncType::PRHA_giveFinish),
        mTptr(ca) {}

    void call() override {
        const stdptr<eProvideResourceHelpAction> t = mTptr;
        if(!t) return;
        t->finishGiving();
        t->resumeAction();
        if(t && !t->currentAction()) t->rebuildCurrentStage();
    }

protected:
    void serializeFields(SaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eProvideResourceHelpAction> mTptr;
};

#endif // EPROVIDERESOURCEHELPACTION_H
