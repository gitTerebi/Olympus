#ifndef EHADESHELPACTION_H
#define EHADESHELPACTION_H

#include "characters/gods/actions/god-action.h"
#include "fileIO/esavearchive.h"

enum class eHadesHelpStage {
    none, appear, goTo, give, giving, disappear
};

class eSaveArchive;

class eHadesHelpAction : public eGodAction {
public:
    eHadesHelpAction(eCharacter* const c);

    bool decide() override;

    static bool sHelpNeeded(const eCityId cid,
                            const GameBoard& board);

    void rebuildCurrentStage();
    void finishGiving();
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void goToTarget();
    void give();
    void spawnGiveMissile(eBuilding* const target);

    eHadesHelpStage mStage{eHadesHelpStage::none};
    stdptr<eBuilding> mTarget;
    eHadesHelpStage mPreGivingStage{eHadesHelpStage::none};
};

class eHdHA_giveFinish : public eCharActFunc {
public:
    eHdHA_giveFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::HdHA_giveFinish) {}
    eHdHA_giveFinish(GameBoard& board, eHadesHelpAction* const ca) :
        eCharActFunc(board, eCharActFuncType::HdHA_giveFinish),
        mTptr(ca) {}

    void call() override {
        const stdptr<eHadesHelpAction> t = mTptr;
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
    stdptr<eHadesHelpAction> mTptr;
};

class eGodProvideDrachmasAct : public eGodAct {
public:
    eGodProvideDrachmasAct(GameBoard& board, const eCityId cid) :
        eGodAct(board, eGodActType::provideDrachmas),
        mCityId(cid) {}
    eGodProvideDrachmasAct(GameBoard& board) :
        eGodProvideDrachmasAct(board, eCityId::neutralFriendly) {}

    eMissileTarget find(eTile* const t) override {
        (void)t;
        const auto null = static_cast<eTile*>(nullptr);
        return null;
    }

    void act() override {
        board().addResource(mCityId, eResourceType::drachmas, 1500);
    }

    void serializeFields(eSaveArchive& ar) override {
        ar.field("cityId", mCityId);
    }
private:
    eCityId mCityId;
};

#endif // EHADESHELPACTION_H
