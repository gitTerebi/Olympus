#include "elavaevent.h"
#include "fileIO/esavearchive.h"

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

void eLavaEvent::serializeFields(eSaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    ePointEventValue::serialize(ar);
    eGodEventValue::serialize(ar);
    eGodReasonEventValue::serialize(ar);
}

void eLavaEvent::loadResources() const {
    eGameTextures::loadLava();
}
