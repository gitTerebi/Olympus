#include "egameundo.h"

#include "game-board.h"
#include "buildings/ebuilding.h"
#include "engine/boardData/ecityfinances.h"
#include "enumbers.h"

eGameUndo::eGameUndo(GameBoard& board) :
    mBoard(board) {}

void eGameUndo::startBuild(eBuildingType type) {
    mState.valid = false;
    mState.tiles.clear();
    mState.placed.clear();
    mState.cost = 0;
}

void eGameUndo::finishBuild() {
    mState.valid = mState.placed.size() > 0;
    if(mState.valid) mState.timeoutTicks = 10 * eNumbers::sDayLength;
}

void eGameUndo::incTime(const int by) {
    if(!mState.valid) return;
    mState.timeoutTicks -= by;
    if(mState.timeoutTicks <= 0) {
        mState.valid = false;
        mState.placed.clear();
        mState.tiles.clear();
        mState.cost = 0;
    }
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