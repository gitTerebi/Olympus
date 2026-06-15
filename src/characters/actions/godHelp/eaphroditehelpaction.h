#ifndef EAPHRODITEHELPACTION_H
#define EAPHRODITEHELPACTION_H

#include "characters/gods/actions/god-action.h"

#include "buildings/ehousebase.h"

class SaveArchive;

enum class eAphroditeHelpStage {
    none, appear, goTo, populate, populating, disappear
};

class eAphroditeHelpAction : public eGodAction {
public:
    eAphroditeHelpAction(eCharacter* const c);

    bool decide() override;

    static bool sHelpNeeded(const eCityId cid,
                            const GameBoard& board);

    void rebuildCurrentStage();
    void finishPopulating();
protected:
    void serializeFields(SaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    eHouseBase* nearestHouseWithVacancies();

    void goToTarget();
    void populate();
    void spawnPopulateMissile(eHouseBase* const target);

    eAphroditeHelpStage mStage{eAphroditeHelpStage::none};
    eAphroditeHelpStage mPrePopulatingStage{eAphroditeHelpStage::none};
    stdptr<eHouseBase> mPopulateTarget;
};

class eAHA_populateFinish : public eCharActFunc {
public:
    eAHA_populateFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::AHA_populateFinish) {}
    eAHA_populateFinish(GameBoard& board, eAphroditeHelpAction* const ca) :
        eCharActFunc(board, eCharActFuncType::AHA_populateFinish),
        mTptr(ca) {}

    void call() override {
        const stdptr<eAphroditeHelpAction> t = mTptr;
        if(!t) return;
        t->finishPopulating();
        t->resumeAction();
        if(t && !t->currentAction()) t->rebuildCurrentStage();
    }

protected:
    void serializeFields(SaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eAphroditeHelpAction> mTptr;
};

class eAphroditeHelpAct : public eGodAct {
public:
    eAphroditeHelpAct(GameBoard& board,
                      eHouseBase* const target) :
        eGodAct(board, eGodActType::aphroditeHelp),
        mTarget(target) {}

    eAphroditeHelpAct(GameBoard& board) :
        eAphroditeHelpAct(board, nullptr) {}

    eMissileTarget find(eTile* const t) override {
        (void)t;
        const auto null = static_cast<eTile*>(nullptr);
        return null;
    }

    void act() override {
        if(!mTarget) return;
        const int v = mTarget->vacancies();
        mTarget->moveIn(v);
    }

    void serializeFields(SaveArchive& ar) override {
        ar.buildingAsField("targetHouse", &board(), mTarget);
    }
private:
    stdptr<eHouseBase> mTarget;
};

#endif // EAPHRODITEHELPACTION_H
