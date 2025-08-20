#include "eearthquakeevent.h"

#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "elanguage.h"

eEarthquakeEvent::eEarthquakeEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::earthquake,
               branch, board),
    ePointEventValue(eBannerTypeS::disasterPoint,
                    cid, board) {}

void eEarthquakeEvent::trigger() {
    const auto board = gameBoard();
    if(!board) return;
    choosePointId();
    chooseCount();
    const auto cid = cityId();
    const int pt = pointId();
    const auto startTile = board->disasterTile(cid, pt);
    if(!startTile) return;
    eEventData ed(cid);
    ed.fGod = god();
    ed.fTile = startTile;
    board->earthquake(startTile, count());
    const auto e = godReason() ? eEvent::earthquakeGod :
                                 eEvent::earthquake;
    board->event(e, ed);
}

std::string eEarthquakeEvent::longName() const {
    return eLanguage::text("earthquake_long_name");
}

void eEarthquakeEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    ePointEventValue::write(dst);
    eCountEventValue::write(dst);
    eGodEventValue::write(dst);
    eGodReasonEventValue::write(dst);
}

void eEarthquakeEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    ePointEventValue::read(src);
    eCountEventValue::read(src);
    eGodEventValue::read(src);
    eGodReasonEventValue::read(src);
}
