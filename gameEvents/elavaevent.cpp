#include "elavaevent.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

#include "elanguage.h"

#include "engine/e-game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"

bool validLavaEventTile(eTile* const tile) {
    return tile->lavaZone();
}

eLavaEvent::eLavaEvent(
    const eCityId cid,
    const eGameEventBranch branch,
    eGameBoard& board) :
    eGameEvent(cid, eGameEventType::lavaFlow,
               branch, board),
    ePointEventValue(eBannerTypeS::disasterPoint,
                     cid, board, validLavaEventTile) {}

void eLavaEvent::trigger() {
    const auto board = gameBoard();
    if(!board) return;
    choosePointId();
    const auto cid = cityId();
    const int pt = pointId();
    const auto startTile = board->disasterTile(cid, pt);
    if(!startTile) return;
    eEventData ed(cid);
    ed.fTile = startTile;
    board->addLavaFlow(startTile);
    const auto e = godReason() ? eEvent::lavaFlowGod :
                                 eEvent::lavaFlow;
    board->event(e, ed);

    const auto b = board->banner(cid, eBannerTypeS::disasterPoint, pt);
    const auto t = b ? b->tile() : nullptr;
    if(t) t->removeBanner(b);
}

std::string eLavaEvent::longName() const {
    return eLanguage::zeusText(48, 68);
}

void eLavaEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    ePointEventValue::write(dst);
    eGodEventValue::write(dst);
    eGodReasonEventValue::write(dst);
    eSaveArchive ar(dst);
    const_cast<eLavaEvent*>(this)->serialize(ar);
}

void eLavaEvent::serialize(eSaveArchive& ar) {

}

void eLavaEvent::serializeJson(eJsonArchive& ar) {
    eGameEvent::serializeJson(ar);
    ePointEventValue::serializeJson(ar);
    eGodEventValue::serializeJson(ar);
    eGodReasonEventValue::serializeJson(ar);
}

void eLavaEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    ePointEventValue::read(src);
    eGodEventValue::read(src);
    eGodReasonEventValue::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eLavaEvent::loadResources() const {
    eGameTextures::loadLava();
}
