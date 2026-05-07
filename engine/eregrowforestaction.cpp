#include "eregrowforestaction.h"

#include "engine/etile.h"
#include "engine/egameboard.h"
#include "fileIO/esavearchive.h"

eRegrowForestAction::eRegrowForestAction(eTile* const tile) :
    ePlannedAction(false, 500000, ePlannedActionType::regrowForest),
    mTile(tile) {}

eRegrowForestAction::eRegrowForestAction() :
    eRegrowForestAction(nullptr) {}

void eRegrowForestAction::trigger(eGameBoard& board) {
    (void)board;
    if(mTile->terrain() != eTerrain::choppedForest) return;
    mTile->setTerrain(eTerrain::forest);
}

void eRegrowForestAction::read(eReadStream& src, eGameBoard& board) {
    ePlannedAction::read(src, board);
    eSaveArchive ar(src);
    serialize(ar, &board);
}

void eRegrowForestAction::write(eWriteStream& dst) const {
    ePlannedAction::write(dst);
    eSaveArchive ar(dst);
    const_cast<eRegrowForestAction*>(this)->serialize(ar, nullptr);
}

void eRegrowForestAction::serialize(eSaveArchive& ar, eGameBoard* board) {
    if(ar.reading()) {
        ar.tile(mTile, *board);
    } else {
        ar.writeStream().writeTile(mTile);
    }
}
