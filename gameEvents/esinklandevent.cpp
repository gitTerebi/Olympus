#include "esinklandevent.h"

#include "elanguage.h"

#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"

eSinkLandEvent::eSinkLandEvent(
    const eCityId cid,
    const eGameEventBranch branch,
    eGameBoard& board) :
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

void eSinkLandEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eCountEventValue::write(dst);
    eGodEventValue::write(dst);
    eGodReasonEventValue::write(dst);
}

void eSinkLandEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    eCountEventValue::read(src);
    eGodEventValue::read(src);
    eGodReasonEventValue::read(src);
}

void eSinkLandEvent::loadResources() const {
    eGameTextures::loadWave();
}
