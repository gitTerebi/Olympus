#include "player-conquest-event-base.h"

#include "engine/game-board.h"

PlayerConquestEventBase::PlayerConquestEventBase(
        const eCityId cid,
        const eGameEventType type,
        const eGameEventBranch branch,
        GameBoard& board) :
    ArmyEventBase(cid, type, branch, board) {
    board.addConquest(this);
}

PlayerConquestEventBase::~PlayerConquestEventBase() {
    removeConquestEvent();
}

void PlayerConquestEventBase::addAres() {
    mForces.fAres = true;
}

void PlayerConquestEventBase::removeConquestEvent() {
    const auto board = gameBoard();
    if(!board) return;
    board->removeConquest(this);
}
