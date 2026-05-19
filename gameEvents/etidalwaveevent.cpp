#include "etidalwaveevent.h"
#include "fileIO/esavearchive.h"

#include "elanguage.h"

#include "engine/e-game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"

bool validWaveEventTile(eTile* const tile) {
    return tile->tidalWaveZone();
}

eTidalWaveEvent::eTidalWaveEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::tidalWave,
               branch, board),
    ePointEventValue(eBannerTypeS::disasterPoint,
                    cid, board, validWaveEventTile) {}

void eTidalWaveEvent::trigger() {
    const auto board = gameBoard();
    if(!board) return;
    choosePointId();
    const auto cid = cityId();
    const int pt = pointId();
    const auto startTile = board->disasterTile(cid, pt);
    if(!startTile) return;
    eEventData ed(cid);
    ed.fGod = god();
    ed.fTile = startTile;
    board->addTidalWave(startTile, mPermanent);
    const auto e = godReason() ? eEvent::tidalWaveGod :
                                 eEvent::tidalWave;
    board->event(e, ed);
    if(mPermanent) {
        const auto b = board->banner(cid, eBannerTypeS::disasterPoint, pt);
        const auto t = b ? b->tile() : nullptr;
        if(t) t->removeBanner(b);
    }
}

std::string eTidalWaveEvent::longName() const {
    return eLanguage::zeusText(48, 69);
}

void eTidalWaveEvent::serializeFields(eSaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    ePointEventValue::serialize(ar);
    eGodEventValue::serialize(ar);
    eGodReasonEventValue::serialize(ar);
    ar.field("permanent", mPermanent, false);
}

void eTidalWaveEvent::loadResources() const {
    eGameTextures::loadWave();
}
