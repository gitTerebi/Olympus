#ifndef EHERAHELPACTION_H
#define EHERAHELPACTION_H

#include "characters/gods/actions/egodaction.h"

#include "buildings/eagorabase.h"

class eSaveArchive;

enum class eHeraHelpStage {
    none, appear, goTo, give, disappear
};

class eHeraHelpAction : public eGodAction {
public:
    eHeraHelpAction(eCharacter* const c);

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

    eHeraHelpStage mStage{eHeraHelpStage::none};
    stdptr<eAgoraBase> mTarget;
    std::vector<stdptr<eAgoraBase>> mFutureTargets;
};

class eGodProvideAgoraAct : public eGodAct {
public:
    eGodProvideAgoraAct(eGameBoard& board,
                        eAgoraBase* const target) :
        eGodAct(board, eGodActType::provideAgora),
        mTarget(target) {}

    eGodProvideAgoraAct(eGameBoard& board) :
        eGodProvideAgoraAct(board, nullptr) {}

    eMissileTarget find(eTile* const t) override {
        (void)t;
        const auto null = static_cast<eTile*>(nullptr);
        return null;
    }

    void act() override {
        if(mTarget) {
            mTarget->add(eResourceType::food, 8);
            mTarget->add(eResourceType::fleece, 8);
            mTarget->add(eResourceType::oliveOil, 8);
            mTarget->add(eResourceType::wine, 8);
        }
    }

    void read(eReadStream& src) override {
        src.readBuilding(&board(), [this](eBuilding* const b) {
            mTarget = static_cast<eAgoraBase*>(b);
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeBuilding(mTarget);
    }

    void serializeJson(eJsonArchive& ar) override {
        eBuilding* raw = mTarget.get();
        ar.buildingRef("mTarget", raw, board());
        if(ar.reading()) mTarget = static_cast<eAgoraBase*>(raw);
    }
private:
    stdptr<eAgoraBase> mTarget;
};

#endif // EHERAHELPACTION_H
