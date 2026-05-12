#include "eplannedaction.h"

#include "fileIO/ereadstream.h"
#include "fileIO/ewritestream.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

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

void ePlannedAction::read(eReadStream& src, eGameBoard& board) {
    (void)board;
    eSaveArchive ar(src);
    serialize(ar);
}

void ePlannedAction::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<ePlannedAction*>(this)->serialize(ar);
}

void ePlannedAction::serialize(eSaveArchive& ar) {
    ar.field("mRecurring", mRecurring);
    ar.field("mActionTime", mActionTime);
    ar.field("mFinished", mFinished);
    ar.field("mTime", mTime);
}

void ePlannedAction::serializeJson(eJsonArchive& ar, eGameBoard& board) {
    (void)board;
    ar.field("mRecurring", mRecurring);
    ar.field("mActionTime", mActionTime);
    ar.field("mFinished", mFinished);
    ar.field("mTime", mTime);
}

ePlannedAction* ePlannedAction::sCreate(const ePlannedActionType type) {
    switch(type) {
    case ePlannedActionType::regrowForest:
        return new eRegrowForestAction();
    case ePlannedActionType::colonyMonument:
        return new eColonyMonumentAction();
    }
}

void ePlannedAction::incTime(const int by, eGameBoard& board) {
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
