#include "ecolonymonumentaction.h"

#include "engine/e-worldcity.h"
#include "engine/game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "fileIO/esavearchive.h"

eColonyMonumentAction::eColonyMonumentAction(
        const stdsptr<eWorldCity>& city) :
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

void eColonyMonumentAction::serializeFields(eSaveArchive& ar, GameBoard* board) {
    ePlannedAction::serializeFields(ar, board);
    ar.city(board, mCity);
}
