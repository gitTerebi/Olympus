#ifndef EHERAHELPACTION_H
#define EHERAHELPACTION_H

#include "characters/gods/actions/god-action.h"

#include "buildings/eagorabase.h"

class SaveArchive;

enum class eHeraHelpStage {
    none, appear, goTo, give, giving, disappear
};

class eHeraHelpAction : public eGodAction {
public:
    eHeraHelpAction(eCharacter* const c);

    bool decide() override;

    static bool sHelpNeeded(const eCityId cid,
                            const GameBoard& board);
protected:
    void serializeFields(SaveArchive& ar) override;
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
    eHrHA_giveFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::HrHA_giveFinish) {}
    eHrHA_giveFinish(GameBoard& board, eHeraHelpAction* const ca) :
        eCharActFunc(board, eCharActFuncType::HrHA_giveFinish),
        mTptr(ca) {}

    void call() override {
        const stdptr<eHeraHelpAction> t = mTptr;
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
    stdptr<eHeraHelpAction> mTptr;
};

class eGodProvideAgoraAct : public eGodAct {
public:
    eGodProvideAgoraAct(GameBoard& board,
                        eAgoraBase* const target) :
        eGodAct(board, eGodActType::provideAgora),
        mTarget(target) {}

    eGodProvideAgoraAct(GameBoard& board) :
        eGodProvideAgoraAct(board, nullptr) {}

    eMissileTarget find(eTile* const t) override {
        (void)t;
        const auto null = static_cast<eTile*>(nullptr);
        return null;
    }

    void act() override {
        if(mTarget) {
            mTarget->add(eResourceType::food, 8);
            mTarget->add(eResourceType::fleece, 8);
            mTarget->add(eResourceType::oliveOil, 8);
            mTarget->add(eResourceType::wine, 8);
        }
    }

    void serializeFields(SaveArchive& ar) override {
        ar.buildingAsField("targetAgora", &board(), mTarget);
    }
private:
    stdptr<eAgoraBase> mTarget;
};

#endif // EHERAHELPACTION_H
