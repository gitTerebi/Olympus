#include "ecolonymonumentaction.h"

#include "engine/e-worldcity.h"
#include "engine/e-game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "fileIO/esavearchive.h"

eColonyMonumentAction::eColonyMonumentAction(
        const stdsptr<eWorldCity>& city) :
    ePlannedAction(false, 1000, ePlannedActionType::colonyMonument),
    mCity(city) {}

eColonyMonumentAction::eColonyMonumentAction() :
    eColonyMonumentAction(nullptr) {}

void eColonyMonumentAction::trigger(eGameBoard& board) {
    const auto capital = board.currentCity();
    const auto cid = capital->cityId();
    eEventData ed(cid);
    ed.fCity = mCity;
    board.event(eEvent::colonyMonument, ed);
    board.allow(cid, eBuildingType::commemorative, 2);
}

void eColonyMonumentAction::read(eReadStream& src, eGameBoard& board) {
    ePlannedAction::read(src, board);
    eSaveArchive ar(src);
    serialize(ar, &board);
}

void eColonyMonumentAction::write(eWriteStream& dst) const {
    ePlannedAction::write(dst);
    eSaveArchive ar(dst);
    const_cast<eColonyMonumentAction*>(this)->serialize(ar, nullptr);
}

void eColonyMonumentAction::serialize(eSaveArchive& ar, eGameBoard* board) {
    if(ar.reading()) {
        ar.readStream().readCity(board, [this](const stdsptr<eWorldCity>& city) {
            mCity = city;
        });
    } else {
        ar.writeStream().writeCity(mCity.get());
    }
}
