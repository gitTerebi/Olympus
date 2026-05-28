#include "eearthquakeevent.h"
#include "fileIO/esavearchive.h"

#include "engine/e-game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "elanguage.h"

eEarthquakeEvent::eEarthquakeEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        GameBoard& board) :
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

void eEarthquakeEvent::serializeFields(eSaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    ePointEventValue::serialize(ar);
    eCountEventValue::serialize(ar);
    eGodEventValue::serialize(ar);
    eGodReasonEventValue::serialize(ar);
}
