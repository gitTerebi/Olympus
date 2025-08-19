#include "etidalwaveevent.h"
#include "elanguage.h"

#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"

eTidalWaveEvent::eTidalWaveEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::tidalWave,
               branch, board),
    ePointEventBase(eBannerTypeS::disasterPoint,
                    cid, board) {}

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
}

std::string eTidalWaveEvent::longName() const {
    return eLanguage::text("tidal_wave_long_name");
}

void eTidalWaveEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    ePointEventBase::write(dst);
    eGodEventValue::write(dst);
    eGodReasonEventValue::write(dst);
    dst << mPermanent;
}

void eTidalWaveEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    ePointEventBase::read(src);
    eGodEventValue::read(src);
    eGodReasonEventValue::read(src);
    src >> mPermanent;
}
