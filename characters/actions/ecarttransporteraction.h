#ifndef ECARTTRANSPORTERACTION_H
#define ECARTTRANSPORTERACTION_H

#include "eactionwithcomeback.h"

#include "characters/ecarttransporter.h"
#include "buildings/ebuildingwithresource.h"
#include "fileIO/esavearchive.h"

class eSaveArchive;

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
                    bool preferGranaryFirst);
    void throttleDropoffRetry();
    void goBack();

    void targetResourceAction(const int bx, const int by);
    void targetResourceAction(eBuildingWithResource* const rb);
    int targetProcessTask(eBuildingWithResource* const rb,
                          const eCartTask& task);

    void startResourceAction(const eCartTask& task);
    void finishResourceAction(const eCartTask& task);

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

    virtual void serializeFields(eSaveArchive& ar);

    // subclass hook — called when findTarget BFS fails
    virtual void onFindTargetFail() {}
    // subclass hook — called when findTarget BFS succeeds and path starts
    virtual void onFoundTarget() {}

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
    stdsptr<eWalkableObject> getWalkable() const;

    void updateWaiting();

    void spread();

    void disappear();

    int mUpdateWaiting = 0;

    int mRetryCount = 0;
    static const int kMaxDropoffRetries = 250;
    static const int kRetryWaitTicks = 1000;

    eCartState mState = eCartState::idle;
};

class eCTA_findTargetFinish : public eCharActFunc {
public:
    eCTA_findTargetFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::CTA_findTargetFinish) {}
    eCTA_findTargetFinish(eGameBoard& board,
                          eCartTransporterAction* const ca) :
        eCharActFunc(board, eCharActFuncType::CTA_findTargetFinish),
        mTptr(ca) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        t->targetResourceAction(mBx, mBy);
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eCartTransporterAction*>(ca);
        });
        eSaveArchive ar(src);
        ar.field("buildingX", mBx);
        ar.field("buildingY", mBy);
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mTptr);
        eSaveArchive ar(dst);
        ar.field("buildingX", const_cast<int&>(mBx));
        ar.field("buildingY", const_cast<int&>(mBy));
    }

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
    eCTA_waitOutsideFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::CTA_waitOutsideFinish) {}
    eCTA_waitOutsideFinish(eGameBoard& board,
                           eCartTransporterAction* const ca) :
        eCharActFunc(board, eCharActFuncType::CTA_waitOutsideFinish),
        mTptr(ca) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        t->spread();
    }

    void read(eReadStream& src) override {
        src.readCharacterAction(&board(), [this](eCharacterAction* const ca) {
            mTptr = static_cast<eCartTransporterAction*>(ca);
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacterAction(mTptr);
    }
private:
    stdptr<eCartTransporterAction> mTptr;
};

class eCTA_spreadFinish : public eCharActFunc {
public:
    eCTA_spreadFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::CTA_spreadFinish) {}
    eCTA_spreadFinish(eGameBoard& board, eCharacter* const c) :
        eCharActFunc(board, eCharActFuncType::CTA_spreadFinish),
        mCptr(c) {}

    void call() override {
        if(!mCptr) return;
        const auto c = mCptr.get();
        c->setActionType(eCharacterActionType::stand);
    }

    void read(eReadStream& src) override {
        src.readCharacter(&board(), [this](eCharacter* const c) {
            mCptr = c;
        });
    }

    void write(eWriteStream& dst) const override {
        dst.writeCharacter(mCptr);
    }
private:
    stdptr<eCharacter> mCptr;
};

#endif // ECARTTRANSPORTERACTION_H
