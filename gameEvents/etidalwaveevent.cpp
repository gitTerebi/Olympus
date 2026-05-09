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

void eTidalWaveEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    ePointEventValue::write(dst);
    eGodEventValue::write(dst);
    eGodReasonEventValue::write(dst);
    eSaveArchive ar(dst);
    const_cast<eTidalWaveEvent*>(this)->serialize(ar);
}

void eTidalWaveEvent::serialize(eSaveArchive& ar) {
    ar.field("mPermanent", mPermanent);
}

void eTidalWaveEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    ePointEventValue::read(src);
    eGodEventValue::read(src);
    eGodReasonEventValue::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eTidalWaveEvent::loadResources() const {
    eGameTextures::loadWave();
}
