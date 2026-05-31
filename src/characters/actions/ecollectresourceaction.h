#ifndef ECOLLECTRESOURCEACTION_H
#define ECOLLECTRESOURCEACTION_H

#include "eactionwithcomeback.h"

#include "emovepathaction.h"
#include "engine/emovedirection.h"
#include "ecollectaction.h"
#include "ewalkablehelpers.h"
#include "walkable/ehasresourceobject.h"
#include "buildings/eresourcecollectbuildingbase.h"

class eMovePathAction;

class eResourceCollectorBase;
class eResourceCollectBuildingBase;
class eSaveArchive;

enum class eTileActionType {
    none,
    masonry,
    blackMasonry
};

enum class eCollectResourceActionStage {
    idle, findingResource, collecting, goingBack, waiting
};

class eCollectResourceAction : public eActionWithComeback {
public:
    eCollectResourceAction(eResourceCollectBuildingBase* const b,
                           eCharacter* const c,
                           const stdsptr<eHasResourceObject>& hr);
    eCollectResourceAction(eCharacter* const c);

    bool decide() override;

    void setCollectedAction(const eTileActionType a);
    void callCollectedAction(eTile* const tile) const;

    void setGetAtTile(const bool b) { mGetAtTile = b; }
    void setAddResource(const bool b) { mAddResource = b; }
    void setWalkable(const stdsptr<eWalkableObject> & w) { mWalkable = w; }

    void setFinishOnce(const bool f) { mFinishOnce = f; }
    void setWaitTime(const int w) { mWaitTime = w; }

    void setDisabled(const bool d) { mDisabled = d; }
    void finishCollecting(eTile* const tile);
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    bool findResourceDecision();
    bool collect(eTile* const tile);
    void goBackDecision();
    void waitDecision();

    stdsptr<eHasResourceObject> mHasResource;
    eResourceCollectBuildingBase* mBuilding = nullptr;
    eTileActionType mCollectedAction = eTileActionType::none;
    stdsptr<eWalkableObject> mWalkable =
            eWalkableObject::sCreateDefault();

    bool mDisabled = false;
    int mWaitTime = 5000;

    bool mFinishOnce = true;

    bool mAddResource = true;

    bool mGetAtTile = true;

    bool mNoTarget = false;
    eCollectResourceActionStage mStage = eCollectResourceActionStage::idle;
    eTile* mTargetTile = nullptr;
};

class eCRA_collectFinish : public eCharActFunc {
public:
    eCRA_collectFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::CRA_collectFinish) {}
    eCRA_collectFinish(GameBoard& board,
                       eCollectResourceAction* const ca,
                       eTile* const tile) :
        eCharActFunc(board, eCharActFuncType::CRA_collectFinish),
        mPtr(ca), mTile(tile) {}

    void call() override {
        if(!mTile) return;
        if(mPtr) {
            const auto t = mPtr.get();
            t->finishCollecting(mTile);
        } else {
            mTile->setBusy(false);
        }
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.tileField("tile", board(), mTile);
        ar.characterActionAsField("target", &board(), mPtr);
    }
private:
    stdptr<eCollectResourceAction> mPtr;
    eTile* mTile = nullptr;
};

class eCRA_collectFail : public eCharActFunc {
public:
    eCRA_collectFail(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::CRA_collectFail) {}
    eCRA_collectFail(GameBoard& board, eTile* const tile) :
        eCharActFunc(board, eCharActFuncType::CRA_collectFail),
        mTile(tile) {}

    void call() override {
        if(!mTile) return;
        mTile->setBusy(false);
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.tileField("tile", board(), mTile);
    }
private:
    eTile* mTile = nullptr;
};

class eCRA_callCollectedActionFinish : public eCharActFunc {
public:
    eCRA_callCollectedActionFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::CRA_callCollectedActionFinish) {}
    eCRA_callCollectedActionFinish(GameBoard& board,
                                   eResourceCollectBuildingBase* const b) :
        eCharActFunc(board, eCharActFuncType::CRA_callCollectedActionFinish),
        mBptr(b) {}

    void call() override {
        if(!mBptr) return;
        mBptr->addRaw();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.buildingAsField("collectBuilding", &board(), mBptr);
    }
private:
    stdptr<eResourceCollectBuildingBase> mBptr;
};

#endif // ECOLLECTRESOURCEACTION_H
