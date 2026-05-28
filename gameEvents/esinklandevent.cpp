#include "esinklandevent.h"
#include "fileIO/esavearchive.h"

#include "elanguage.h"

#include "engine/e-game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"

eSinkLandEvent::eSinkLandEvent(
    const eCityId cid,
    const eGameEventBranch branch,
    GameBoard& board) :
    eGameEvent(cid, eGameEventType::sinkLand,
               branch, board) {}

void eSinkLandEvent::trigger() {
    const auto board = gameBoard();
    if(!board) return;
    chooseCount();
    const auto cid = cityId();
    eEventData ed(cid);
    ed.fGod = god();
    board->sinkLand(cid, mCount);
    const auto e = godReason() ? eEvent::sinkLandGod :
                                 eEvent::sinkLand;
    board->event(e, ed);
}

std::string eSinkLandEvent::longName() const {
    return eLanguage::zeusText(156, 28);
}

void eSinkLandEvent::serializeFields(eSaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    eCountEventValue::serialize(ar);
    eGodEventValue::serialize(ar);
    eGodReasonEventValue::serialize(ar);
}

void eSinkLandEvent::loadResources() const {
    eGameTextures::loadWave();
}
