#ifndef EHADESHELPACTION_H
#define EHADESHELPACTION_H

#include "characters/gods/actions/egodaction.h"
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
                            const eGameBoard& board);

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
    eHdHA_giveFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::HdHA_giveFinish) {}
    eHdHA_giveFinish(eGameBoard& board, eHadesHelpAction* const ca) :
        eCharActFunc(board, eCharActFuncType::HdHA_giveFinish),
        mTptr(ca) {}

    void call() override {
        const stdptr<eHadesHelpAction> t = mTptr;
        if(!t) return;
        t->finishGiving();
        t->resumeAction();
        if(t && !t->currentAction()) t->rebuildCurrentStage();
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eHadesHelpAction*>(ca);
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mTptr);
    }
private:
    stdptr<eHadesHelpAction> mTptr;
};

class eGodProvideDrachmasAct : public eGodAct {
public:
    eGodProvideDrachmasAct(eGameBoard& board, const eCityId cid) :
        eGodAct(board, eGodActType::provideDrachmas),
        mCityId(cid) {}
    eGodProvideDrachmasAct(eGameBoard& board) :
        eGodProvideDrachmasAct(board, eCityId::neutralFriendly) {}

    eMissileTarget find(eTile* const t) {
        (void)t;
        const auto null = static_cast<eTile*>(nullptr);
        return null;
    }

    void act() {
        board().addResource(mCityId, eResourceType::drachmas, 1500);
    }

    void read(eReadStream& src) {
        eSaveArchive ar(src);
        ar.field("cityId", mCityId);
    }

    void write(eWriteStream& dst) const {
        eSaveArchive ar(dst);
        ar.field("cityId", const_cast<eCityId&>(mCityId));
    }
private:
    eCityId mCityId;
};

#endif // EHADESHELPACTION_H
