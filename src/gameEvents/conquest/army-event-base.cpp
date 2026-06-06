#include "army-event-base.h"

#include "engine/game-board.h"

#include "army-return-event.h"
#include "enumbers.h"
#include "fileIO/esavearchive.h"

ArmyEventBase::ArmyEventBase(const eCityId cid,
                             const eGameEventType type,
                             const eGameEventBranch branch,
                             GameBoard& board) :
    eGameEvent(cid, type, branch, board) {
    board.addArmyEvent(this);
}

ArmyEventBase::~ArmyEventBase() {
    removeArmyEvent();
}

void ArmyEventBase::removeArmyEvent() {
    const auto board = gameBoard();
    if(!board) return;
    board->removeArmyEvent(this);
}

void ArmyEventBase::serializeFields(eSaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    const auto board = gameBoard();
    ar.archiveField("forces", [this, board](eSaveArchive& childAr) {
        mForces.serialize(childAr, board);
    });
    ar.worldCityField("city", board, mCity);
}

void ArmyEventBase::planArmyReturn() {
    planArmyReturn(eNumbers::sArmyTravelTime);
}

void ArmyEventBase::planArmyReturn(const int travelTime) {
    const auto board = gameBoard();
    if(!board) return;
    const auto forces = mForces.splitIntoCities();
    for(const auto& f : forces) {
        const auto e = e::make_shared<ArmyReturnEvent>(
            f.first, eGameEventBranch::root, *board);
        const auto boardDate = board->date();
        const auto date = boardDate + travelTime;
        e->initializeDate(date, travelTime, 1);
        e->initialize(f.second, mCity);
        addConsequence(e);
    }
}
