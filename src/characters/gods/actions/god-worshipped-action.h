#ifndef EGODWORSHIPPEDACTION_H
#define EGODWORSHIPPEDACTION_H

#include "characters/gods/actions/god-action.h"

class eSaveArchive;

enum class GodWorshippedStage {
    none, appear, goTo1, patrol1, goTo2, patrol2, disappear, defend,
    huntMonster, fightMonster
};

class GodWorshippedAction : public eGodAction {
public:
    GodWorshippedAction(eCharacter* const c);

    void increment(const int by) override;
    bool decide() override;

    void lookForMonster();
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void rebuildCurrentStage();

    void defendCity();
    void lookForMonsterFight();
    bool fightMonster(eMonster* const m);
    void huntMonster(eMonster* const m, const bool second);

    GodWorshippedStage mStage{GodWorshippedStage::none};

    int mLookForBless = 0;
    int mLookForSoldierAttack = 0;
    int mLookForCityDefense = 0;
    int mLookForMonster = 0;
};

class eGWA_huntMonsterFinish : public eCharActFunc {
public:
    eGWA_huntMonsterFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GWA_huntMonsterFinish) {}
    eGWA_huntMonsterFinish(GameBoard& board, GodWorshippedAction* const ca) :
        eCharActFunc(board, eCharActFuncType::GWA_huntMonsterFinish),
        mTptr(ca) {}

    void call() override {
        if(!mTptr) return;
        mTptr->lookForMonster();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<GodWorshippedAction> mTptr;
};

#endif // EGODWORSHIPPEDACTION_H
