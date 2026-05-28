#ifndef GOD_MINION_ACTION_H
#define GOD_MINION_ACTION_H

#include "characters/gods/actions/egodmonsteraction.h"

class eSaveArchive;
class eMonster;

enum class eGodMinionStage {
    none, appear, goTo1, patrol1, goTo2, patrol2, disappear,
    huntMonster, fightMonster
};

class eGodMinionAction : public eGodMonsterAction {
public:
    eGodMinionAction(eCharacter* const c);

    void increment(const int by) override;
    bool decide() override;

    void lookForMonster();
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void rebuildCurrentStage();

    void lookForMonsterFight();
    bool fightMonster(eMonster* const m);
    void huntMonster(eMonster* const m, const bool second);

    eGodMinionStage mStage{eGodMinionStage::none};

    int mLookForMonster = 0;
};

class eGMinA_huntMonsterFinish : public eCharActFunc {
public:
    eGMinA_huntMonsterFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GMinA_huntMonsterFinish) {}
    eGMinA_huntMonsterFinish(eGameBoard& board, eGodMinionAction* const ca) :
        eCharActFunc(board, eCharActFuncType::GMinA_huntMonsterFinish),
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
    stdptr<eGodMinionAction> mTptr;
};

#endif // GOD_MINION_ACTION_H
