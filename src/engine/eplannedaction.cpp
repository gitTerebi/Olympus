#include "eplannedaction.h"

#include "fileIO/save-archive.h"

#include "eregrowforestaction.h"
#include "ecolonymonumentaction.h"

ePlannedAction::ePlannedAction(const bool recurring,
                               const int actionTime,
                               const ePlannedActionType type) :
    mType(type), mRecurring(recurring),
    mActionTime(actionTime) {}

ePlannedAction::ePlannedAction(const ePlannedActionType type) :
    mType(type) {}

ePlannedAction::~ePlannedAction() {}

void ePlannedAction::serialize(SaveArchive& ar, GameBoard* board) {
    serializeFields(ar, board);
}

void ePlannedAction::serializeFields(SaveArchive& ar, GameBoard* board) {
    (void)board;
    ar.field("recurring", mRecurring);
    ar.field("actionTime", mActionTime);
    ar.field("finished", mFinished);
    ar.field("time", mTime);
}

ePlannedAction* ePlannedAction::sCreate(const ePlannedActionType type) {
    switch(type) {
    case ePlannedActionType::regrowForest:
        return new eRegrowForestAction();
    case ePlannedActionType::colonyMonument:
        return new eColonyMonumentAction();
    }
    return nullptr;
}

void ePlannedAction::incTime(const int by, GameBoard& board) {
    if(mFinished) return;
    mTime += by;
    while(mTime > mActionTime) {
        mTime -= mActionTime;
        trigger(board);
        if(!mRecurring) {
            mFinished = true;
            return;
        }
    }
}
