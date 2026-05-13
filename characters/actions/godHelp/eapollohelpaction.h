#ifndef EAPOLLOHELPACTION_H
#define EAPOLLOHELPACTION_H

#include "characters/gods/actions/egodaction.h"

#include "buildings/esmallhouse.h"

class eSaveArchive;

enum class eApolloHelpStage {
    none, appear, goTo, heal, disappear
};

class eApolloHelpAction : public eGodAction {
public:
    eApolloHelpAction(eCharacter* const c);

    bool decide() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(eJsonArchive& ar) override;

    static bool sHelpNeeded(const eCityId cid,
                            const eGameBoard& board);
private:
    void serialize(eSaveArchive& ar);
    void goToTarget();
    void heal();

    eApolloHelpStage mStage{eApolloHelpStage::none};
};

class eApolloHelpAct : public eGodAct {
public:
    eApolloHelpAct(eGameBoard& board,
                   eSmallHouse* const target) :
        eGodAct(board, eGodActType::apolloHelp),
        mTarget(target) {}

    eApolloHelpAct(eGameBoard& board) :
        eApolloHelpAct(board, nullptr) {}

    eMissileTarget find(eTile* const t) override {
        (void)t;
        const auto null = static_cast<eTile*>(nullptr);
        return null;
    }

    void act() override {
        auto& board = eGodAct::board();
        const auto p = board.plagueForHouse(mTarget);
        if(p) board.healPlague(p);
    }

    void read(eReadStream& src) override {
        src.readBuilding(&board(), [this](eBuilding* const b) {
            mTarget = static_cast<eSmallHouse*>(b);
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeBuilding(mTarget);
    }

    void serializeJson(eJsonArchive& ar) override {
        eBuilding* raw = mTarget.get();
        ar.buildingRef("mTarget", raw, board());
        if(ar.reading()) mTarget = static_cast<eSmallHouse*>(raw);
    }
private:
    stdptr<eSmallHouse> mTarget;
};

#endif // EAPOLLOHELPACTION_H
