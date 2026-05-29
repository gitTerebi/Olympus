#ifndef SOLDIER_ACTION_H
#define SOLDIER_ACTION_H

#include "fighting-action.h"

#include "characters/echaracter.h"
#include "walkable/eobsticlehandler.h"
#include "buildings/ebuilding.h"

class eSoldier;

class eSaveArchive;

enum class SoldierActionStage {
    idle, banner, home, abroad
};

class SoldierAction : public FightingAction {
public:
    SoldierAction(eCharacter* const c);

    bool decide() override;

    void increment(const int by) override;

    void goHome() override;
    void goAbroad() override;
    void goBackToBanner(eOrientation facing,
                        const eAction& findFailAct = nullptr,
                        const eAction& findFinishAct = nullptr);

    static eBuilding* sFindHome(const eCharacterType t,
                                const eCityId cid,
                                const GameBoard& brd);

    void setSpreadPeriod(const bool s) { mSpreadPeriod = s; }
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
    eTile* repositionAnchor() const override;
private:
    stdsptr<eObsticleHandler> obsticleHandler() override;
    void rebuildCurrentStage();
    bool tooFarFromBanner() const;
    bool enemyNear() const;
    void tickBannerReturn(const int by);

    int mGoToBannerCountdown = 0;
    bool mSpreadPeriod = false; // for spreading invasion forces
    bool mArrivedAtBanner = false;
    SoldierActionStage mStage = SoldierActionStage::idle;
};

class SoldierObsticleHandler : public eObsticleHandler {
public:
    SoldierObsticleHandler(GameBoard& board) :
        eObsticleHandler(board, eObsticleHandlerType::soldier) {}
    SoldierObsticleHandler(GameBoard& board,
                            SoldierAction* const t) :
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

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<SoldierAction> mTptr;
};

class SA_goHomeFinish : public eCharActFunc {
public:
    SA_goHomeFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::SA_goHomeFinish) {}
    SA_goHomeFinish(GameBoard& board, eCharacter* const c) :
        eCharActFunc(board, eCharActFuncType::SA_goHomeFinish),
        mCptr(c) {}

    void call() override {
        if(!mCptr) return;
        mCptr->kill();
    }
protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterField("character", &board(), mCptr);
    }
private:
    stdptr<eCharacter> mCptr;
};

#endif // SOLDIER_ACTION_H
