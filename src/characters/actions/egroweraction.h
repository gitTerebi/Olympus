#ifndef EGROWERACTION_H
#define EGROWERACTION_H

#include "eactionwithcomeback.h"

#include "characters/egrower.h"
#include "buildings/egrowerslodge.h"
#include "buildings/eresourcebuilding.h"
#include "fileIO/esavearchive.h"

class eSaveArchive;

enum class eGrowerActionStage {
    idle, findingResource, working, goingBack, waiting
};

enum class eGrowerActionMode {
    normal, oliveHarvester, oliveGroomer
};

class eGrowerAction : public eActionWithComeback {
    friend class eGRA_workOnDecisionFinish;
public:
    eGrowerAction(const eGrowerType type,
                  eGrowersLodge* const lodge,
                  eCharacter* const c,
                  const eGrowerActionMode mode = eGrowerActionMode::normal);
    eGrowerAction(eCharacter* const c);

    bool decide() override;
    void increment(const int by) override;
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void rebuildCurrentStage();
    bool findResourceDecision();
    void workOnDecision(eTile* const tile);
    void finishWorkOn(eTile* const tile, const eBuildingType type);
    void releaseWorkTile();
    void goBackDecision();
    void waitDecision();

    eGrowerType mType;
    eGrower* mGrower = nullptr;
    eGrowersLodge* mLodge = nullptr;

    bool mFinishOnce = true;

    int mGroomed = 0;
    bool mNoResource = false;
    eGrowerActionMode mMode = eGrowerActionMode::normal;
    int mOliveGroomsThisMonth = 0;
    int mOliveGroomMonth = -1;
    eGrowerActionStage mStage = eGrowerActionStage::idle;
    int mWaitRemaining = 0;
    int mWorkRemaining = 0;
    eTile* mTargetTile = nullptr;
    eBuildingType mTargetBuildingType = eBuildingType::none;
};

class eGRA_workOnDecisionFinish : public eCharActFunc {
public:
    eGRA_workOnDecisionFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GRA_workOnDecisionFinish) {}
    eGRA_workOnDecisionFinish(GameBoard& board, eGrowerAction* const ca,
                              eTile* const tile, const eBuildingType type) :
        eCharActFunc(board, eCharActFuncType::GRA_workOnDecisionFinish),
        mTptr(ca), mTile(tile), mType(type) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        t->finishWorkOn(mTile, mType);
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
        ar.tileField("tile", board(), mTile);
        ar.field("buildingType", mType);
    }
private:
    stdptr<eGrowerAction> mTptr;
    eTile* mTile = nullptr;
    eBuildingType mType;
};

class eGRA_workOnDecisionDeleteFail : public eCharActFunc {
public:
    eGRA_workOnDecisionDeleteFail(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::GRA_workOnDecisionDeleteFail) {}
    eGRA_workOnDecisionDeleteFail(GameBoard& board, eTile* const tile) :
        eCharActFunc(board, eCharActFuncType::GRA_workOnDecisionDeleteFail),
        mTile(tile) {}

    void call() override {
        mTile->setBusy(false);
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.tileField("tile", board(), mTile);
    }
private:
    eTile* mTile = nullptr;
};

#endif // EGROWERACTION_H
