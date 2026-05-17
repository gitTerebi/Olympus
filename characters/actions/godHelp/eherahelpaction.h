#ifndef EHERAHELPACTION_H
#define EHERAHELPACTION_H

#include "characters/gods/actions/egodaction.h"

#include "buildings/eagorabase.h"

class eSaveArchive;

enum class eHeraHelpStage {
    none, appear, goTo, give, giving, disappear
};

class eHeraHelpAction : public eGodAction {
public:
    eHeraHelpAction(eCharacter* const c);

    bool decide() override;

    static bool sHelpNeeded(const eCityId cid,
                            const eGameBoard& board);
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    void goToTarget();
    void give();
    void spawnGiveMissile(eAgoraBase* const target);

    eHeraHelpStage mStage{eHeraHelpStage::none};
    stdptr<eAgoraBase> mTarget;
    std::vector<stdptr<eAgoraBase>> mFutureTargets;
    eHeraHelpStage mPreGivingStage{eHeraHelpStage::none};
    stdptr<eAgoraBase> mGiveTarget;

public:
    void rebuildCurrentStage();
    void finishGiving();
protected:
    void resumeFromSavedState() override;
};

class eHrHA_giveFinish : public eCharActFunc {
public:
    eHrHA_giveFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::HrHA_giveFinish) {}
    eHrHA_giveFinish(eGameBoard& board, eHeraHelpAction* const ca) :
        eCharActFunc(board, eCharActFuncType::HrHA_giveFinish),
        mTptr(ca) {}

    void call() override {
        const stdptr<eHeraHelpAction> t = mTptr;
        if(!t) return;
        t->finishGiving();
        t->resumeAction();
        if(t && !t->currentAction()) t->rebuildCurrentStage();
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eHeraHelpAction*>(ca);
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mTptr);
    }
private:
    stdptr<eHeraHelpAction> mTptr;
};

class eGodProvideAgoraAct : public eGodAct {
public:
    eGodProvideAgoraAct(eGameBoard& board,
                        eAgoraBase* const target) :
        eGodAct(board, eGodActType::provideAgora),
        mTarget(target) {}

    eGodProvideAgoraAct(eGameBoard& board) :
        eGodProvideAgoraAct(board, nullptr) {}

    eMissileTarget find(eTile* const t) {
        (void)t;
        const auto null = static_cast<eTile*>(nullptr);
        return null;
    }

    void act() {
        if(mTarget) {
            mTarget->add(eResourceType::food, 8);
            mTarget->add(eResourceType::fleece, 8);
            mTarget->add(eResourceType::oliveOil, 8);
            mTarget->add(eResourceType::wine, 8);
        }
    }

    void read(eReadStream& src) {
        src.readBuilding(&board(), [this](eBuilding* const b) {
            mTarget = static_cast<eAgoraBase*>(b);
        });
    }

    void write(eWriteStream& dst) const {
        dst.writeBuilding(mTarget);
    }
private:
    stdptr<eAgoraBase> mTarget;
};

#endif // EHERAHELPACTION_H
