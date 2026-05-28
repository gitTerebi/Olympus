#include "earmyeventbase.h"

#include "engine/e-game-board.h"

#include "earmyreturnevent.h"
#include "enumbers.h"
#include "fileIO/esavearchive.h"

eArmyEventBase::eArmyEventBase(const eCityId cid,
                               const eGameEventType type,
                               const eGameEventBranch branch,
                               GameBoard& board) :
    eGameEvent(cid, type, branch, board) {
    board.addArmyEvent(this);
}

eArmyEventBase::~eArmyEventBase() {
    removeArmyEvent();
}

void eArmyEventBase::removeArmyEvent() {
    const auto board = gameBoard();
    if(!board) return;
    board->removeArmyEvent(this);
}

void eArmyEventBase::serializeFields(eSaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    const auto board = gameBoard();
    ar.archiveField("forces", [this, board](eSaveArchive& childAr) {
        mForces.serialize(childAr, board);
    });
    ar.worldCityField("city", board, mCity);
}

void eArmyEventBase::planArmyReturn() {
    planArmyReturn(eNumbers::sArmyTravelTime);
}

void eArmyEventBase::planArmyReturn(const int travelTime) {
    const auto board = gameBoard();
    if(!board) return;
    const auto forces = mForces.splitIntoCities();
    for(const auto& f : forces) {
        const auto e = e::make_shared<eArmyReturnEvent>(
            f.first, eGameEventBranch::root, *board);
        const auto boardDate = board->date();
        const auto date = boardDate + travelTime;
        e->initializeDate(date, travelTime, 1);
        e->initialize(f.second, mCity);
        addConsequence(e);
    }
}
