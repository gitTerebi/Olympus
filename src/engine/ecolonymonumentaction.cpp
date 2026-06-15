#include "ecolonymonumentaction.h"

#include "engine/world-city.h"
#include "engine/game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "fileIO/save-archive.h"

eColonyMonumentAction::eColonyMonumentAction(
        const stdsptr<WorldCity>& city) :
    ePlannedAction(false, 1000, ePlannedActionType::colonyMonument),
    mCity(city) {}

eColonyMonumentAction::eColonyMonumentAction() :
    eColonyMonumentAction(nullptr) {}

void eColonyMonumentAction::trigger(GameBoard& board) {
    const auto capital = board.currentCity();
    const auto cid = capital->cityId();
    eEventData ed(cid);
    ed.fCity = mCity;
    board.event(eEvent::colonyMonument, ed);
    board.allow(cid, eBuildingType::commemorative, 2);
}

void eColonyMonumentAction::serializeFields(SaveArchive& ar, GameBoard* board) {
    ePlannedAction::serializeFields(ar, board);
    ar.city(board, mCity);
}
