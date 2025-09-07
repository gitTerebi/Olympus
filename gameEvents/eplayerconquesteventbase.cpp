#include "eplayerconquesteventbase.h"

#include "engine/egameboard.h"

ePlayerConquestEventBase::ePlayerConquestEventBase(
        const eCityId cid,
        const eGameEventType type,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eArmyEventBase(cid, type, branch, board) {
    board.addConquest(this);
}

ePlayerConquestEventBase::~ePlayerConquestEventBase() {
    removeConquestEvent();
}

void ePlayerConquestEventBase::addAres() {
    mForces.fAres = true;
}

void ePlayerConquestEventBase::removeConquestEvent() {
    const auto board = gameBoard();
    if(!board) return;
    board->removeConquest(this);
}
