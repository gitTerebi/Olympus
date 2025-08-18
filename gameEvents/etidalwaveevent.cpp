#include "etidalwaveevent.h"
#include "elanguage.h"

#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"

eTidalWaveEvent::eTidalWaveEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::tidalWave, branch, board) {}

void eTidalWaveEvent::trigger() {
    const auto board = gameBoard();
    if(!board) return;
    const auto cid = cityId();
    const auto startTile = board->disasterTile(cid, mDisasterPoint);
    if(!startTile) return;
    eEventData ed(cid);
    ed.fTile = startTile;
    board->addTidalWave(startTile, mPermanent);
    board->event(eEvent::tidalWave, ed);
}

std::string eTidalWaveEvent::longName() const {
    return eLanguage::text("tidal_wave_long_name");
}

void eTidalWaveEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    dst << mDisasterPoint;
    dst << mPermanent;
}

void eTidalWaveEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    src >> mDisasterPoint;
    src >> mPermanent;
}
