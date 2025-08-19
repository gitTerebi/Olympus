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
    ePointEventBase(eBannerTypeS::disasterPoint,
                    cid, board) {}

void eEarthquakeEvent::trigger() {
    const auto board = gameBoard();
    if(!board) return;
    choosePointId();
    const auto cid = cityId();
    const int pt = pointId();
    const auto startTile = board->disasterTile(cid, pt);
    if(!startTile) return;
    eEventData ed(cid);
    ed.fGod = mGod;
    ed.fTile = startTile;
    board->earthquake(startTile, mSize);
    board->event(eEvent::earthquake, ed);
}

std::string eEarthquakeEvent::longName() const {
    return eLanguage::text("earthquake_long_name");
}

void eEarthquakeEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    ePointEventBase::write(dst);
    dst << mGod;
    dst << mSize;
}

void eEarthquakeEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    ePointEventBase::read(src);
    src >> mGod;
    src >> mSize;
}
