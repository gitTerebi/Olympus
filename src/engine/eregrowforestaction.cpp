#include "eregrowforestaction.h"

#include "engine/etile.h"
#include "engine/game-board.h"
#include "fileIO/save-archive.h"

eRegrowForestAction::eRegrowForestAction(eTile* const tile) :
    ePlannedAction(false, 500000, ePlannedActionType::regrowForest),
    mTile(tile) {}

eRegrowForestAction::eRegrowForestAction() :
    eRegrowForestAction(nullptr) {}

void eRegrowForestAction::trigger(GameBoard& board) {
    (void)board;
    if(mTile->terrain() != eTerrain::choppedForest) return;
    mTile->setTerrain(eTerrain::forest);
}

void eRegrowForestAction::serializeFields(SaveArchive& ar, GameBoard* board) {
    ePlannedAction::serializeFields(ar, board);
    ar.tile(mTile, *board);
}
