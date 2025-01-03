#ifndef EHADESHELPACTION_H
#define EHADESHELPACTION_H

#include "characters/actions/egodaction.h"

enum class eHadesHelpStage {
    none, appear, goTo, give, disappear
};

class eHadesHelpAction : public eGodAction {
public:
    eHadesHelpAction(eCharacter* const c);

    bool decide() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

    static bool sHelpNeeded(const eCityId cid,
                            const eGameBoard& board);
private:
    void goToTarget();
    void give();

    eHadesHelpStage mStage{eHadesHelpStage::none};
    stdptr<eBuilding> mTarget;
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
        src >> mCityId;
    }

    void write(eWriteStream& dst) const {
        dst << mCityId;
    }
private:
    eCityId mCityId;
};

#endif // EHADESHELPACTION_H
