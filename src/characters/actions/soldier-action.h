#ifndef SOLDIER_ACTION_H
#define SOLDIER_ACTION_H

#include "fighting-action.h"

#include "characters/echaracter.h"
#include "walkable/eobsticlehandler.h"

class eBuilding;
class eSoldier;

class eSaveArchive;

enum class SoldierActionStage {
    idle, banner, home, abroad, chase
};

class SoldierAction : public FightingAction {
public:
    SoldierAction(eCharacter* const c);

    bool decide() override;

    void increment(const int by) override;

    void goHome() override;
    void goAbroad() override;
    void beingAttacked(int ttx, int tty) override;
    void setCombatBlockage(eBuilding* const b);
    void goBackToBanner(eOrientation facing,
                        const eAction& findFailAct = nullptr,
                        const eAction& findFinishAct = nullptr);

    static eBuilding* sFindHome(const eCharacterType t,
                                const eCityId cid,
                                const GameBoard& brd);

    void setSpreadPeriod(const bool s) { mSpreadPeriod = s; }
    void setDepartDelay(const int ms) { mDepartDelay = ms; }
    void cancelAndClearAction() {
        mSpreadPeriod = false;
        cancelAttack();
        clearSavedMovement();
        setCurrentAction(nullptr);
        character()->setActionType(eCharacterActionType::stand);
    }
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
    eTile* repositionAnchor() const override;
private:
    bool prefersPathAround() const override;
    bool allowsSelfPositioning() const override;
    stdsptr<eObsticleHandler> obsticleHandler() override;
    void rebuildCurrentStage();
    bool enemyNear() const;
    void tickBannerReturn(const int by);
    bool followBannerDirector();

    int mGoToBannerCountdown = 0;
    int mPathFailCount = 0;
    eTile* mLastFailTile = nullptr;
    int mFollowDirectorCooldown = 0;
    bool mSpreadPeriod = false; // for spreading invasion forces
    bool mArrivedAtBanner = false;
    SoldierActionStage mStage = SoldierActionStage::idle;
    int mDepartDelay = 0;
    eTile* mChaseTarget = nullptr;
};

class SoldierObsticleHandler : public eObsticleHandler {
public:
    SoldierObsticleHandler(GameBoard& board) :
        eObsticleHandler(board, eObsticleHandlerType::soldier) {}
    SoldierObsticleHandler(GameBoard& board,
                            SoldierAction* const t) :
        eObsticleHandler(board, eObsticleHandlerType::soldier),
        mTptr(t) {}

    bool handle(eTile* const tile) override;

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
