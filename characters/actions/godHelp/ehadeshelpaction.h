#ifndef EHADESHELPACTION_H
#define EHADESHELPACTION_H

#include "characters/gods/actions/egodaction.h"
#include "fileIO/esavearchive.h"

enum class eHadesHelpStage {
    none, appear, goTo, give, disappear
};

class eSaveArchive;

class eHadesHelpAction : public eGodAction {
public:
    eHadesHelpAction(eCharacter* const c);

    bool decide() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(eJsonArchive& ar) override;

    static bool sHelpNeeded(const eCityId cid,
                            const eGameBoard& board);
private:
    void serialize(eSaveArchive& ar);

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

    eMissileTarget find(eTile* const t) override {
        (void)t;
        const auto null = static_cast<eTile*>(nullptr);
        return null;
    }

    void act() override {
        board().addResource(mCityId, eResourceType::drachmas, 1500);
    }

    void read(eReadStream& src) override {
        eSaveArchive ar(src);
        ar.field("cityId", mCityId);
    }

    void write(eWriteStream& dst) const override {
        eSaveArchive ar(dst);
        ar.field("cityId", const_cast<eCityId&>(mCityId));
    }

    void serializeJson(eJsonArchive& ar) override {
        ar.field("mCityId", mCityId);
    }
private:
    eCityId mCityId;
};

#endif // EHADESHELPACTION_H
