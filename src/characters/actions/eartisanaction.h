#ifndef EARTISANACTION_H
#define EARTISANACTION_H

#include "eactionwithcomeback.h"

#include "characters/eartisan.h"
#include "buildings/eartisansguild.h"
#include "buildings/sanctuaries/esanctbuilding.h"

class SaveArchive;

enum class eArtisanActionStage {
    idle, findingTarget, working, goingBack, waiting
};

class eArtisanAction : public eActionWithComeback {
public:
    eArtisanAction(eCharacter* const c, eArtisansGuild* const guild);
    eArtisanAction(eCharacter* const c);

    bool decide() override;
    void increment(const int by) override;
protected:
    void serializeFields(SaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void rebuildCurrentStage();
    bool findTargetDecision();
    void workOnDecision(eTile* const tile);
    void finishWork();
    void releaseWorkTarget();
    void goBackDecision();

    eArtisansGuild* mGuild = nullptr;

    bool mNoTarget = false;
    eArtisanActionStage mStage = eArtisanActionStage::idle;
    int mWaitRemaining = 0;
    int mWorkRemaining = 0;
    eTile* mTargetTile = nullptr;
    stdptr<eSanctBuilding> mTargetBuilding;
};

class eArtA_buildFinish : public eCharActFunc {
public:
    eArtA_buildFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::ArtA_buildFinish) {}
    eArtA_buildFinish(GameBoard& board, eSanctBuilding* const b) :
        eCharActFunc(board, eCharActFuncType::ArtA_buildFinish),
        mBptr(b) {}

    void call() override {
        if(!mBptr) return;
        const auto b = mBptr.get();
        b->setWorkedOn(false);
        if(b->resourcesAvailable()) {
            b->incProgress();
        }
    }

protected:
    void serializeFields(SaveArchive& ar) override {
        ar.buildingAsField("sanctuary", &board(), mBptr);
    }
private:
    stdptr<eSanctBuilding> mBptr;
};

class eArtA_buildDelete : public eCharActFunc {
public:
    eArtA_buildDelete(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::ArtA_buildDelete) {}
    eArtA_buildDelete(GameBoard& board, eSanctBuilding* const b) :
        eCharActFunc(board, eCharActFuncType::ArtA_buildDelete),
        mBptr(b) {}

    void call() override {
        if(!mBptr) return;
        const auto b = mBptr.get();
        b->setWorkedOn(false);
    }

protected:
    void serializeFields(SaveArchive& ar) override {
        ar.buildingAsField("sanctuary", &board(), mBptr);
    }
private:
    stdptr<eSanctBuilding> mBptr;
};

#endif // EARTISANACTION_H
