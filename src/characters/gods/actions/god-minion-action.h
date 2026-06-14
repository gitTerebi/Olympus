#ifndef GOD_MINION_ACTION_H
#define GOD_MINION_ACTION_H

#include "characters/gods/actions/god-monster-action.h"

class eSaveArchive;
class eMonster;

enum class eGodMinionStage {
    none, appear, goTo1, patrol1, goTo2, patrol2, disappear,
    huntMonster, fightMonster,
    huntSoldier, fightSoldier
};

class eGodMinionAction : public GodMonsterAction {
public:
    eGodMinionAction(eCharacter* const c);

    void increment(const int by) override;
    bool decide() override;

    //void lookForMonster();
    void lookForSoldier();
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void rebuildCurrentStage();

    //void lookForMonsterFight();
    //bool fightMonster(eMonster* const m);
    //void huntMonster(eMonster* const m, const bool second);

    void lookForSoldierFight();
    bool fightSoldier(eCharacter* const s);
public:
    void fightSoldierContinue(eCharacter* const s);
private:
    void huntSoldier(eCharacter* const s);

    eGodMinionStage mStage{eGodMinionStage::none};

    int mLookForSoldier = 0;
    int mApproachCheck = 0;
    bool mLastFightMelee = false;
};

class eGMinA_huntMonsterFinish : public eCharActFunc {
public:
    eGMinA_huntMonsterFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GMinA_huntMonsterFinish) {}
    eGMinA_huntMonsterFinish(GameBoard& board, eGodMinionAction* const ca) :
        eCharActFunc(board, eCharActFuncType::GMinA_huntMonsterFinish),
        mTptr(ca) {}

    void call() override {
        if(!mTptr) return;
        mTptr->lookForSoldier();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eGodMinionAction> mTptr;
};

class eGMinA_huntSoldierFinish : public eCharActFunc {
public:
    eGMinA_huntSoldierFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GMinA_huntSoldierFinish) {}
    eGMinA_huntSoldierFinish(GameBoard& board, eGodMinionAction* const ca) :
        eCharActFunc(board, eCharActFuncType::GMinA_huntSoldierFinish),
        mTptr(ca) {}

    void call() override {
        if(!mTptr) return;
        mTptr->lookForSoldier();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eGodMinionAction> mTptr;
};

class eGMinA_fightSoldierDie : public eCharActFunc {
public:
    eGMinA_fightSoldierDie(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GMinA_fightSoldierDie) {}
    eGMinA_fightSoldierDie(GameBoard& board, eGodMinionAction* const ca,
                           eCharacter* const s, const bool melee) :
        eCharActFunc(board, eCharActFuncType::GMinA_fightSoldierDie),
        mTptr(ca), mSptr(s), mMelee(melee) {}

    void call() override {
        if(!mTptr) return;
        if(mSptr && !mSptr->dead() && mMelee) {
            mSptr->killWithCorpse();
        }
        if(!mSptr || mSptr->dead()) {
            mTptr->lookForSoldier();
        } else {
            mTptr->fightSoldierContinue(mSptr.get());
        }
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("action", &board(), mTptr);
        ar.characterAsField("soldier", &board(), mSptr);
        ar.field("melee", mMelee);
    }
private:
    stdptr<eGodMinionAction> mTptr;
    stdptr<eCharacter> mSptr;
    bool mMelee = false;
};

#endif // GOD_MINION_ACTION_H
