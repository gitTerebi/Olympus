#ifndef ECARTTRANSPORTERACTION_H
#define ECARTTRANSPORTERACTION_H

#include "eactionwithcomeback.h"

#include "characters/ecarttransporter.h"
#include "buildings/ebuildingwithresource.h"
#include "fileIO/esavearchive.h"

class eSaveArchive;
class eThreadBuilding;

enum class eCartState {
    idle,            // at home, nothing to do
    loadingDeliver,  // taking deliver stock from home building
    loadingGet,      // searching for a GET target (no deliver stock)
    waitOutside,     // standing on road outside, waiting to path to target
    movingToTarget,  // pathfinding to target
    atTarget,        // performing exchange at target
    idleOutside,     // on road, delivered, searching for next target
    returning        // walking home
};

class eCartTransporterAction : public eActionWithComeback {
    friend class eCTA_findTargetFinish;
    friend class eCTA_waitOutsideFinish;
public:
    eCartTransporterAction(eCharacter* const c,
                           eBuildingWithResource* const b);
    eCartTransporterAction(eCharacter* const c);

    void increment(const int by) override;
    bool decide() override;

    eCartActionTypeSupport support() const;
    eResourceType supportsResource() const;

    eBuilding* src() const { return mBuilding; }
    eBuilding* target() const { return mTarget; }
    bool noDestination() const { return mState == eCartState::waitOutside; }
    void setBuilding(eBuildingWithResource* b) { mBuilding = b; }
    virtual int cartCapacity(const eResourceType res) const;

    eCartState state() const { return mState; }
    bool waiting();
protected:
    eCartTransporterAction(eCharacter* const c,
                           eBuildingWithResource* const b,
                           const eCharActionType type);

    // FSM enter-state methods
    void enterIdle();
    void enterLoadingDeliver();
    void enterLoadingGet();
    void enterWaitOutside();

    void enterMovingToTarget(const eCartTask& task);
    void enterReturning();

    void findTarget();
    void findTarget(const eCartTask& task);
    void findTarget(const eCartTask& task, eBuilding* avoided);
    void findTarget(const std::vector<eCartTask>& tasks);
    void findTarget(const std::vector<eCartTask>& tasks, eBuilding* avoided);
    void findTarget(const std::vector<eCartTask>& tasks,
                    eBuilding* avoided,
                    bool preferGranaryFirst,
                    bool preferEmptyFirst = true,
                    bool preferGetsFirst = true);
    void throttleDropoffRetry();
    void goBack();

    void targetResourceAction(const int bx, const int by);
    void targetResourceAction(eBuildingWithResource* const rb);
    int targetProcessTask(eBuildingWithResource* const rb,
                          const eCartTask& task);

    virtual void startResourceAction(const eCartTask& task);
    void finishResourceAction(const eCartTask& task);
    virtual bool acceptsTargetForTask(const eCartTask& task,
                                      const eThreadBuilding& target) const;

    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
    virtual bool savesCartState() const;

    // subclass hook — called when findTarget BFS fails
    virtual void onFindTargetFail();
    // subclass hook — called when findTarget BFS succeeds and path starts
    virtual void onFoundTarget() {}
    // subclass hook — called when movement reaches target and exchange runs
    virtual void onAtTarget() {}

    eBuildingWithResource* building() const { return mBuilding.get(); }
    eCartTransporter* cart() const {
        return static_cast<eCartTransporter*>(character());
    }

    stdptr<eBuildingWithResource> mBuilding;
    stdptr<eBuilding> mTarget;
    eCartTask mTask;

    void waitOutside();
    void clearTask();

private:
    stdsptr<eWalkableObject> getWalkable(bool excludeHomeRect = false) const;
    stdsptr<eWalkableObject> getWalkableForTask(bool excludeHomeRect,
                                                eCartActionType taskType) const;

    void updateWaiting();

    void spread();

    void disappear();

    int mUpdateWaiting = 0;

    int mRetryCount = 0;
    static const int kMaxDropoffRetries = 250;
    static const int kRetryWaitTicks = 1000;

protected:
    eCartState mState = eCartState::idle;
};

class eCTA_findTargetFinish : public eCharActFunc {
public:
    eCTA_findTargetFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::CTA_findTargetFinish) {}
    eCTA_findTargetFinish(GameBoard& board,
                          eCartTransporterAction* const ca) :
        eCharActFunc(board, eCharActFuncType::CTA_findTargetFinish),
        mTptr(ca) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        t->targetResourceAction(mBx, mBy);
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
        ar.field("buildingX", mBx);
        ar.field("buildingY", mBy);
    }
public:
    void setXY(const int x, const int y) {
        mBx = x;
        mBy = y;
    }
private:
    stdptr<eCartTransporterAction> mTptr;
    int mBx = -1;
    int mBy = -1;
};

class eCTA_waitOutsideFinish : public eCharActFunc {
public:
    eCTA_waitOutsideFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::CTA_waitOutsideFinish) {}
    eCTA_waitOutsideFinish(GameBoard& board,
                           eCartTransporterAction* const ca) :
        eCharActFunc(board, eCharActFuncType::CTA_waitOutsideFinish),
        mTptr(ca) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        t->spread();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eCartTransporterAction> mTptr;
};

class eCTA_spreadFinish : public eCharActFunc {
public:
    eCTA_spreadFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::CTA_spreadFinish) {}
    eCTA_spreadFinish(GameBoard& board, eCharacter* const c) :
        eCharActFunc(board, eCharActFuncType::CTA_spreadFinish),
        mCptr(c) {}

    void call() override {
        if(!mCptr) return;
        const auto c = mCptr.get();
        c->setActionType(eCharacterActionType::stand);
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterField("character", &board(), mCptr);
    }
private:
    stdptr<eCharacter> mCptr;
};

#endif // ECARTTRANSPORTERACTION_H
