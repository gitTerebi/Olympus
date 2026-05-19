#include "eregrowforestaction.h"

#include "engine/etile.h"
#include "engine/e-game-board.h"
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

void eRegrowForestAction::serializeFields(eSaveArchive& ar, eGameBoard* board) {
    ePlannedAction::serializeFields(ar, board);
    ar.tile(mTile, *board);
}
