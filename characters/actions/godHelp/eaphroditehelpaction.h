#ifndef EAPHRODITEHELPACTION_H
#define EAPHRODITEHELPACTION_H

#include "characters/gods/actions/egodaction.h"

#include "buildings/ehousebase.h"

class eSaveArchive;

enum class eAphroditeHelpStage {
    none, appear, goTo, populate, disappear
};

class eAphroditeHelpAction : public eGodAction {
public:
    eAphroditeHelpAction(eCharacter* const c);

    bool decide() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(eJsonArchive& ar) override;

    static bool sHelpNeeded(const eCityId cid,
                            const eGameBoard& board);
private:
    void serialize(eSaveArchive& ar);
    eHouseBase* nearestHouseWithVacancies();

    void goToTarget();
    void populate();

    eAphroditeHelpStage mStage{eAphroditeHelpStage::none};
};

class eAphroditeHelpAct : public eGodAct {
public:
    eAphroditeHelpAct(eGameBoard& board,
                      eHouseBase* const target) :
        eGodAct(board, eGodActType::aphroditeHelp),
        mTarget(target) {}

    eAphroditeHelpAct(eGameBoard& board) :
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

    void read(eReadStream& src) override {
        src.readBuilding(&board(), [this](eBuilding* const b) {
            mTarget = static_cast<eHouseBase*>(b);
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeBuilding(mTarget);
    }

    void serializeJson(eJsonArchive& ar) override {
        eBuilding* raw = mTarget.get();
        ar.buildingRef("mTarget", raw, board());
        if(ar.reading()) mTarget = static_cast<eHouseBase*>(raw);
    }
private:
    stdptr<eHouseBase> mTarget;
};

#endif // EAPHRODITEHELPACTION_H
