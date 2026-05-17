#ifndef EAPHRODITEHELPACTION_H
#define EAPHRODITEHELPACTION_H

#include "characters/gods/actions/egodaction.h"

#include "buildings/ehousebase.h"

class eSaveArchive;

enum class eAphroditeHelpStage {
    none, appear, goTo, populate, populating, disappear
};

class eAphroditeHelpAction : public eGodAction {
public:
    eAphroditeHelpAction(eCharacter* const c);

    bool decide() override;

    static bool sHelpNeeded(const eCityId cid,
                            const eGameBoard& board);

    void rebuildCurrentStage();
    void finishPopulating();
protected:
    void serializeFields(eSaveArchive& ar) override;
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
    eAHA_populateFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::AHA_populateFinish) {}
    eAHA_populateFinish(eGameBoard& board, eAphroditeHelpAction* const ca) :
        eCharActFunc(board, eCharActFuncType::AHA_populateFinish),
        mTptr(ca) {}

    void call() override {
        const stdptr<eAphroditeHelpAction> t = mTptr;
        if(!t) return;
        t->finishPopulating();
        t->resumeAction();
        if(t && !t->currentAction()) t->rebuildCurrentStage();
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eAphroditeHelpAction*>(ca);
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mTptr);
    }
private:
    stdptr<eAphroditeHelpAction> mTptr;
};

class eAphroditeHelpAct : public eGodAct {
public:
    eAphroditeHelpAct(eGameBoard& board,
                      eHouseBase* const target) :
        eGodAct(board, eGodActType::aphroditeHelp),
        mTarget(target) {}

    eAphroditeHelpAct(eGameBoard& board) :
        eAphroditeHelpAct(board, nullptr) {}

    eMissileTarget find(eTile* const t) {
        (void)t;
        const auto null = static_cast<eTile*>(nullptr);
        return null;
    }

    void act() {
        if(!mTarget) return;
        const int v = mTarget->vacancies();
        mTarget->moveIn(v);
    }

    void read(eReadStream& src) {
        src.readBuilding(&board(), [this](eBuilding* const b) {
            mTarget = static_cast<eHouseBase*>(b);
        });
    }

    void write(eWriteStream& dst) const {
        dst.writeBuilding(mTarget);
    }
private:
    stdptr<eHouseBase> mTarget;
};

#endif // EAPHRODITEHELPACTION_H
