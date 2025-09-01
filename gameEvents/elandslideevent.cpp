#include "elandslideevent.h"

#include "elanguage.h"

#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"

bool validLandSlideEventTile(eTile* const tile) {
    return tile->landSlideZone();
}

eLandSlideEvent::eLandSlideEvent(
    const eCityId cid,
    const eGameEventBranch branch,
    eGameBoard& board) :
    eGameEvent(cid, eGameEventType::landSlide,
               branch, board),
    ePointEventValue(eBannerTypeS::landSlidePoint,
                     cid, board, validLandSlideEventTile) {}

void eLandSlideEvent::trigger() {
    const auto board = gameBoard();
    if(!board) return;
    choosePointId();
    const auto cid = cityId();
    const int pt = pointId();
    const auto startTile = board->landSlideTile(cid, pt);
    if(!startTile) return;
    eEventData ed(cid);
    ed.fTile = startTile;
    board->addLandSlide(startTile);
    board->event(eEvent::landSlide, ed);

    const auto b = board->banner(cid, eBannerTypeS::landSlidePoint, pt);
    const auto t = b ? b->tile() : nullptr;
    if(t) t->removeBanner(b);
}

std::string eLandSlideEvent::longName() const {
    return eLanguage::zeusText(156, 5);
}

void eLandSlideEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    ePointEventValue::write(dst);
}

void eLandSlideEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    ePointEventValue::read(src);
}

void eLandSlideEvent::loadResources() const {
    eGameTextures::loadDust();
}
