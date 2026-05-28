#ifndef EPATROLTARGET_H
#define EPATROLTARGET_H

#include "epatrolbuilding.h"

class eSaveArchive;

class ePatrolTarget : public ePatrolBuilding {
public:
    ePatrolTarget(GameBoard& board,
                  const eBaseTex baseTex,
                  const double overlayX,
                  const double overlayY,
                  const eOverlays overlays,
                  const eCharGenerator& charGen,
                  const eBuildingType type,
                  const int sw, const int sh,
                  const int maxEmployees,
                  const eCityId cid);

    bool isActive() const { return mActiveTimer > 0; }
    bool hasSpawnPool() const { return mSpawnPool > 0; }

    void arrived();

    void timeChanged(const int by) override;
    int spawnCooldown() const override;

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    const eCharGenerator mCharGen;

    int mSpawnPool = 0;
    bool mHadPatroler = false;
    int mActiveTimer = 0;
};

class ePT_spawnGetActorFinish : public eCharActFunc {
public:
    ePT_spawnGetActorFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::PT_spawnGetActorFinish) {}
    ePT_spawnGetActorFinish(GameBoard& board, ePatrolTarget* const t) :
        eCharActFunc(board, eCharActFuncType::PT_spawnGetActorFinish),
        mTptr(t) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        t->arrived();
    }

    ePatrolTarget* target() const { return mTptr.get(); }
protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.buildingAsField("patrolTarget", &board(), mTptr);
    }
private:
    stdptr<ePatrolTarget> mTptr;
};

#endif // EPATROLTARGET_H
