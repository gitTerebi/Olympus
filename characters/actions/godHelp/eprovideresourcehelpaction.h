#ifndef EPROVIDERESOURCEHELPACTION_H
#define EPROVIDERESOURCEHELPACTION_H

#include "characters/gods/actions/egodaction.h"
#include "fileIO/esavearchive.h"

enum class eProvideResourceHelpStage {
    none, appear, goTo, give, disappear
};

class eSaveArchive;

class eProvideResourceHelpAction : public eGodAction {
public:
    eProvideResourceHelpAction(eCharacter* const c,
                               const eCharActionType type,
                               const eResourceType res,
                               const int count);

    bool decide() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(eJsonArchive& ar) override;

    void decCount(const int by);

    static bool sHelpNeeded(const eCityId cid,
                            const eGameBoard& board,
                            const eResourceType res,
                            const int minSpace);
private:
    void serialize(eSaveArchive& ar);

    void goToTarget();
    void give();

    eProvideResourceHelpStage mStage{eProvideResourceHelpStage::none};
    stdptr<eStorageBuilding> mTarget;
    eResourceType mResource = eResourceType::wheat;
    int mCount = 32;
};

class eGodProvideResourceAct : public eGodAct {
public:
    eGodProvideResourceAct(eGameBoard& board,
                           eProvideResourceHelpAction* const action,
                           eStorageBuilding* const target,
                           const eResourceType resource,
                           const int count) :
        eGodAct(board, eGodActType::provideResource),
        mAction(action),
        mTarget(target),
        mResource(resource),
        mCount(count) {}

    eGodProvideResourceAct(eGameBoard& board) :
        eGodProvideResourceAct(board, nullptr, nullptr,
                               eResourceType::meat, 0) {}

    eMissileTarget find(eTile* const t) {
        (void)t;
        const auto null = static_cast<eTile*>(nullptr);
        return null;
    }

    void act() {
        if(mTarget) {
            const int added = mTarget->add(mResource, mCount);
            if(mAction) {
                mAction->decCount(added);
            }
        }
    }

    void read(eReadStream& src) {
        src.readBuilding(&board(), [this](eBuilding* const b) {
            mTarget = static_cast<eStorageBuilding*>(b);
        });
        eSaveArchive ar(src);
        ar.field("resource", mResource);
        ar.field("count", mCount);
        src.readCharacterAction(&board(), [this](eCharacterAction* const a) {
            mAction = static_cast<eProvideResourceHelpAction*>(a);
        });
    }

    void write(eWriteStream& dst) const {
        dst.writeBuilding(mTarget);
        eSaveArchive ar(dst);
        ar.field("resource", const_cast<eResourceType&>(mResource));
        ar.field("count", const_cast<int&>(mCount));
        dst.writeCharacterAction(mAction);
    }

    void serializeJson(eJsonArchive& ar) override {
        eBuilding* rawTarget = mTarget.get();
        ar.buildingRef("mTarget", rawTarget, board());
        if(ar.reading()) mTarget = static_cast<eStorageBuilding*>(rawTarget);
        ar.field("mResource", mResource);
        ar.field("mCount", mCount);
        if(ar.writing()) {
            int ioid = mAction ? mAction->ioID() : -1;
            ar.field("mAction", ioid);
        } else {
            int ioid = -1;
            ar.field("mAction", ioid);
            if(ioid >= 0) {
                ar.addPostFunc([this, ioid]() {
                    mAction = static_cast<eProvideResourceHelpAction*>(
                        board().characterActionWithIOID(ioid));
                });
            }
        }
    }
private:
    stdptr<eProvideResourceHelpAction> mAction;
    stdptr<eStorageBuilding> mTarget;
    eResourceType mResource;
    int mCount;
};

#endif // EPROVIDERESOURCEHELPACTION_H
