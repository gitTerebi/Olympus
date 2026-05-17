#ifndef EPROVIDERESOURCEHELPACTION_H
#define EPROVIDERESOURCEHELPACTION_H

#include "characters/gods/actions/egodaction.h"
#include "fileIO/esavearchive.h"

enum class eProvideResourceHelpStage {
    none, appear, goTo, give, giving, disappear
};

class eSaveArchive;

class eProvideResourceHelpAction : public eGodAction {
public:
    eProvideResourceHelpAction(eCharacter* const c,
                               const eCharActionType type,
                               const eResourceType res,
                               const int count);

    bool decide() override;

    void decCount(const int by);

    static bool sHelpNeeded(const eCityId cid,
                            const eGameBoard& board,
                            const eResourceType res,
                            const int minSpace);

    void rebuildCurrentStage();
    void finishGiving();
protected:
    void serializeFields(eSaveArchive& ar) override;
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
    eGodProvideResourceAct(eGameBoard& board,
                           eProvideResourceHelpAction* const action,
                           eStorageBuilding* const target,
                           const eResourceType resource,
                           const int count) :
        eGodAct(board, eGodActType::provideResource),
        mAction(action),
        mTarget(target),
        mResource(resource),
        mCount(count) {}

    eGodProvideResourceAct(eGameBoard& board) :
        eGodProvideResourceAct(board, nullptr, nullptr,
                               eResourceType::meat, 0) {}

    eMissileTarget find(eTile* const t) {
        (void)t;
        const auto null = static_cast<eTile*>(nullptr);
        return null;
    }

    void act() {
        if(mTarget) {
            const int added = mTarget->add(mResource, mCount);
            if(mAction) {
                mAction->decCount(added);
            }
        }
    }

    void read(eReadStream& src) {
        src.readBuilding(&board(), [this](eBuilding* const b) {
            mTarget = static_cast<eStorageBuilding*>(b);
        });
        eSaveArchive ar(src);
        ar.field("resource", mResource);
        ar.field("count", mCount);
        src.readCharacterAction(&board(), [this](eCharacterAction* const a) {
            mAction = static_cast<eProvideResourceHelpAction*>(a);
        });
    }

    void write(eWriteStream& dst) const {
        dst.writeBuilding(mTarget);
        eSaveArchive ar(dst);
        ar.field("resource", const_cast<eResourceType&>(mResource));
        ar.field("count", const_cast<int&>(mCount));
        dst.writeCharacterAction(mAction);
    }
private:
    stdptr<eProvideResourceHelpAction> mAction;
    stdptr<eStorageBuilding> mTarget;
    eResourceType mResource;
    int mCount;
};

class ePRHA_giveFinish : public eCharActFunc {
public:
    ePRHA_giveFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::PRHA_giveFinish) {}
    ePRHA_giveFinish(eGameBoard& board, eProvideResourceHelpAction* const ca) :
        eCharActFunc(board, eCharActFuncType::PRHA_giveFinish),
        mTptr(ca) {}

    void call() override {
        const stdptr<eProvideResourceHelpAction> t = mTptr;
        if(!t) return;
        t->finishGiving();
        t->resumeAction();
        if(t && !t->currentAction()) t->rebuildCurrentStage();
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eProvideResourceHelpAction*>(ca);
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mTptr);
    }
private:
    stdptr<eProvideResourceHelpAction> mTptr;
};

#endif // EPROVIDERESOURCEHELPACTION_H
