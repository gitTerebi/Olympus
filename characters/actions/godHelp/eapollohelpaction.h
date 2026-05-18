#ifndef EAPOLLOHELPACTION_H
#define EAPOLLOHELPACTION_H

#include "characters/gods/actions/egodaction.h"

#include "buildings/esmallhouse.h"

class eSaveArchive;

enum class eApolloHelpStage {
    none, appear, goTo, heal, healing, disappear
};

class eApolloHelpAction : public eGodAction {
public:
    eApolloHelpAction(eCharacter* const c);

    bool decide() override;

    static bool sHelpNeeded(const eCityId cid,
                            const eGameBoard& board);

    void rebuildCurrentStage();
    void finishHealing();
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void goToTarget();
    void heal();
    void spawnHealMissile(eSmallHouse* const target);

    eApolloHelpStage mStage{eApolloHelpStage::none};
    eApolloHelpStage mPreHealingStage{eApolloHelpStage::none};
    stdptr<eSmallHouse> mHealTarget;
};

class eApHA_healFinish : public eCharActFunc {
public:
    eApHA_healFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::ApHA_healFinish) {}
    eApHA_healFinish(eGameBoard& board, eApolloHelpAction* const ca) :
        eCharActFunc(board, eCharActFuncType::ApHA_healFinish),
        mTptr(ca) {}

    void call() override {
        const stdptr<eApolloHelpAction> t = mTptr;
        if(!t) return;
        t->finishHealing();
        t->resumeAction();
        if(t && !t->currentAction()) t->rebuildCurrentStage();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eApolloHelpAction> mTptr;
};

class eApolloHelpAct : public eGodAct {
public:
    eApolloHelpAct(eGameBoard& board,
                   eSmallHouse* const target) :
        eGodAct(board, eGodActType::apolloHelp),
        mTarget(target) {}

    eApolloHelpAct(eGameBoard& board) :
        eApolloHelpAct(board, nullptr) {}

    eMissileTarget find(eTile* const t) {
        (void)t;
        const auto null = static_cast<eTile*>(nullptr);
        return null;
    }

    void act() {
        auto& board = eGodAct::board();
        const auto p = board.plagueForHouse(mTarget);
        if(p) board.healPlague(p);
    }

    void serializeFields(eSaveArchive& ar) override {
        ar.buildingAsField("targetHouse", &board(), mTarget);
    }
private:
    stdptr<eSmallHouse> mTarget;
};

#endif // EAPOLLOHELPACTION_H
