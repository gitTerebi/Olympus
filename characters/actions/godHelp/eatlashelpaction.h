#ifndef EATLASHELPACTION_H
#define EATLASHELPACTION_H

#include "characters/gods/actions/egodaction.h"

enum class eAtlasHelpStage {
    none, appear, goTo, give, giving, disappear
};

class eSaveArchive;

class eAtlasHelpAction : public eGodAction {
public:
    eAtlasHelpAction(eCharacter* const c);

    bool decide() override;

    static bool sHelpNeeded(const eCityId cid,
                            const eGameBoard& board);

    void rebuildCurrentStage();
    void finishGiving();
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void goToTarget();
    void give();
    void spawnGiveMissile(eMonument* const target);

    eAtlasHelpStage mStage{eAtlasHelpStage::none};
    stdptr<eMonument> mTarget;
    eAtlasHelpStage mPreGivingStage{eAtlasHelpStage::none};
};

class eAtHA_giveFinish : public eCharActFunc {
public:
    eAtHA_giveFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::AtHA_giveFinish) {}
    eAtHA_giveFinish(eGameBoard& board, eAtlasHelpAction* const ca) :
        eCharActFunc(board, eCharActFuncType::AtHA_giveFinish),
        mTptr(ca) {}

    void call() override {
        const stdptr<eAtlasHelpAction> t = mTptr;
        if(!t) return;
        t->finishGiving();
        t->resumeAction();
        if(t && !t->currentAction()) t->rebuildCurrentStage();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eAtlasHelpAction> mTptr;
};

class eAtlasHelpAct : public eGodAct {
public:
    eAtlasHelpAct(eGameBoard& board,
                  eMonument* const target) :
        eGodAct(board, eGodActType::atlasHelp),
        mTarget(target) {}

    eAtlasHelpAct(eGameBoard& board) :
        eAtlasHelpAct(board, nullptr) {}

    eMissileTarget find(eTile* const t) override {
        (void)t;
        const auto null = static_cast<eTile*>(nullptr);
        return null;
    }

    void act() override {
        if(mTarget) {
            mTarget->add(eResourceType::wood, 8);
            const int m = mTarget->add(eResourceType::marble, 12);
            mTarget->add(eResourceType::sculpture, 4);
            if(m < 12) mTarget->add(eResourceType::blackMarble, 12 - m);
            mTarget->add(eResourceType::orichalc, 8);
        }
    }

    void serializeFields(eSaveArchive& ar) override {
        ar.buildingAsField("targetMonument", &board(), mTarget);
    }
private:
    stdptr<eMonument> mTarget;
};

#endif // EATLASHELPACTION_H
