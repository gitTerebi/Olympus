#ifndef EPATROLTARGET_H
#define EPATROLTARGET_H

#include "epatrolbuilding.h"

class SaveArchive;

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

    // active = show timer running; venue spawns patrolers while active.
    bool isActive() const { return mShowDays > 0; }
    // load signal for dispatch (Augustus days_left): more remaining = more
    // recently served, so penalize.
    int showDays() const { return mShowDays; }

    void arrived();

    void timeChanged(const int by) override;
    int spawnCooldown() const override;

protected:
    void serializeFields(SaveArchive& ar) override;
private:
    const eCharGenerator mCharGen;

    int mShowDays = 0;   // remaining days of shows, like Augustus days1
    int mDayAccum = 0;   // tick remainder toward the next day boundary
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
    void serializeFields(SaveArchive& ar) override {
        ar.buildingAsField("patrolTarget", &board(), mTptr);
    }
private:
    stdptr<ePatrolTarget> mTptr;
};

#endif // EPATROLTARGET_H
