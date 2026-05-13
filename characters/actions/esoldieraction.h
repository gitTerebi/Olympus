#ifndef ESOLDIERACTION_H
#define ESOLDIERACTION_H

#include "efightingaction.h"

#include "characters/echaracter.h"
#include "walkable/eobsticlehandler.h"
#include "buildings/ebuilding.h"
#include "engine/e-game-board.h"

class eSoldier;

class eSaveArchive;
class eSoldierAction : public eFightingAction {
public:
    eSoldierAction(eCharacter* const c);

    bool decide() override;

    void increment(const int by) override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

    void goHome() override;
    void goAbroad() override;
    void goBackToBanner(eOrientation facing,
                        const eAction& findFailAct = nullptr,
                        const eAction& findFinishAct = nullptr);

    static eBuilding* sFindHome(const eCharacterType t,
                                const eCityId cid,
                                const eGameBoard& brd);

    void setSpreadPeriod(const bool s) { mSpreadPeriod = s; }
void serializeJson(eJsonArchive& ar) override {
        eFightingAction::serializeJson(ar);
        ar.field("mSpreadPeriod", mSpreadPeriod);
}

private:
    void serialize(eSaveArchive& ar);
    stdsptr<eObsticleHandler> obsticleHandler() override;

    int mGoToBannerCountdown = 0;
    bool mSpreadPeriod = false; // for spreading invasion forces
};

class eSoldierObsticleHandler : public eObsticleHandler {
public:
    eSoldierObsticleHandler(eGameBoard& board) :
        eObsticleHandler(board, eObsticleHandlerType::soldier) {}
    eSoldierObsticleHandler(eGameBoard& board,
                            eSoldierAction* const t) :
        eObsticleHandler(board, eObsticleHandlerType::soldier),
        mTptr(t) {}

    bool handle(eTile* const tile) override {
        if(!mTptr) return false;
        const auto ub = tile->underBuilding();
        if(!ub) return false;
        const auto ubt = ub->type();
        const bool r = eBuilding::sWalkableBuilding(ubt);
        if(r) return false;
        mTptr->attackBuilding(tile, false);
        return true;
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eSoldierAction*>(ca);
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mTptr);
    }
void serializeJson(eJsonArchive& ar) override {
        if(ar.writing()) {
            int _ioid = mTptr ? mTptr->ioID() : -1;
            ar.field("mTptr", _ioid);
        } else {
            int _ioid = -1;
            ar.field("mTptr", _ioid);
            if(_ioid >= 0) ar.addPostFunc([this, _ioid]() { mTptr = static_cast<eSoldierAction*>(board().characterActionWithIOID(_ioid)); });
        }
}

private:
    stdptr<eSoldierAction> mTptr;
};

class eSA_goHomeFinish : public eCharActFunc {
public:
    eSA_goHomeFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::SA_goHomeFinish) {}
    eSA_goHomeFinish(eGameBoard& board, eCharacter* const c) :
        eCharActFunc(board, eCharActFuncType::SA_goHomeFinish),
        mCptr(c) {}

    void call() override {
        if(!mCptr) return;
        mCptr->kill();
    }

    void read(eReadStream& src) override {
        src.readCharacter(&board(), [this](eCharacter* const c) {
            mCptr = c;
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacter(mCptr);
    }

    void serializeJson(eJsonArchive& ar) override {
        if(ar.writing()) {
            int ioid = mCptr ? mCptr->ioID() : -1;
            ar.field("mCptr", ioid);
        } else {
            int ioid = -1;
            ar.field("mCptr", ioid);
            if(ioid >= 0) {
                ar.addPostFunc([this, ioid]() {
                    mCptr = resolveChar(ioid);
                });
            }
        }
    }
private:
    stdptr<eCharacter> mCptr;
};

#endif // ESOLDIERACTION_H
