#include "egameundo.h"

#include "egameboard.h"
#include "buildings/ebuilding.h"
#include "engine/boardData/ecityfinances.h"

eGameUndo::eGameUndo(eGameBoard& board) :
    mBoard(board) {}

void eGameUndo::startBuild(eBuildingType type) {
    mState.valid = false;
    mState.tiles.clear();
    mState.placed.clear();
    mState.cost = 0;
}

void eGameUndo::finishBuild() {
    mState.valid = mState.placed.size() > 0;
}

void eGameUndo::undoLastAction() {
    if(!mState.valid) return;
    mState.valid = false;
    for(const auto b : mState.placed) {
        b->erase();
    }
    mState.placed.clear();
    mState.tiles.clear();
    mBoard.incDrachmas(mBoard.personPlayer(), mState.cost, eFinanceTarget::construction);
    mState.cost = 0;
}