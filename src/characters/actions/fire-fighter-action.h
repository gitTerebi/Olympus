#ifndef FIRE_FIGHTER_ACTION_H
#define FIRE_FIGHTER_ACTION_H

#include "epatrolaction.h"

#include "engine/etile.h"
#include "characters/echaracter.h"

class eSaveArchive;

enum class FireFighterActionStage {
    idle, lookingForFire, puttingOutFire
};

class FireFighterAction : public ePatrolAction {
    friend class FFA_lookForFireFail;
public:
    FireFighterAction(eCharacter* const c,
                      ePatrolBuildingBase* const b,
                      const std::vector<eOrientation>& path,
                      const stdsptr<eDirectionTimes>& dirTimes = {});
    FireFighterAction(eCharacter* const c);

    bool decide() override;
    void increment(const int by) override;
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    bool lookForFire(const bool second);
    void putOutFire(eTile* const tile);

    bool mFireFighting{false};
    int mFireCheck{0};

    int mUsedWater = 0;
    FireFighterActionStage mStage = FireFighterActionStage::idle;
    eTile* mFireTile = nullptr;
};

class FFA_lookForFireFail : public eCharActFunc {
public:
    FFA_lookForFireFail(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::FFA_lookForFireFail) {}
    FFA_lookForFireFail(GameBoard& board,
                        FireFighterAction* const ca) :
        eCharActFunc(board, eCharActFuncType::FFA_lookForFireFail),
        mPtr(ca) {}

    void call() override {
        if(!mPtr) return;
        const auto t = mPtr.get();
        if(!t->mFireFighting) return;
        t->mFireFighting = false;
        const auto c = t->character();
        c->setActionType(eCharacterActionType::walk);
        t->goBackDecision(eWalkableObject::sCreateDefault());
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mPtr);
    }
private:
    stdptr<FireFighterAction> mPtr;
};

class FFA_putOutFireFinish : public eCharActFunc {
public:
    FFA_putOutFireFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::FFA_putOutFireFinish) {}
    FFA_putOutFireFinish(GameBoard& board, eCharacter* const c,
                         eTile* const t) :
        eCharActFunc(board, eCharActFuncType::FFA_putOutFireFinish),
        mCptr(c), mTile(t) {}

    void call() override {
        if(!mCptr) return;
        mCptr->setActionType(eCharacterActionType::stand);
        if(const auto b = mTile->underBuilding()) {
            b->setOnFire(false);
        }
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterField("character", &board(), mCptr);
        ar.tileField("tile", board(), mTile);
    }
private:
    stdptr<eCharacter> mCptr;
    eTile* mTile = nullptr;
};

#endif // FIRE_FIGHTER_ACTION_H
