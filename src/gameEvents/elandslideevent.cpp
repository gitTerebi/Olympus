#include "elandslideevent.h"
#include "fileIO/save-archive.h"

#include "language.h"

#include "engine/game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "characters/soldier-banner.h"

bool validLandSlideEventTile(eTile* const tile) {
    return tile->landSlideZone();
}

eLandSlideEvent::eLandSlideEvent(
    const eCityId cid,
    const eGameEventBranch branch,
    GameBoard& board) :
    eGameEvent(cid, eGameEventType::landSlide,
               branch, board),
    ePointEventValue(BannerTypeS::landSlidePoint,
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

    const auto b = board->banner(cid, BannerTypeS::landSlidePoint, pt);
    const auto t = b ? b->tile() : nullptr;
    if(t) t->removeBanner(b);
}

std::string eLandSlideEvent::longName() const {
    return Language::zeusText(156, 5);
}

void eLandSlideEvent::serializeFields(SaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    ePointEventValue::serialize(ar);
}

void eLandSlideEvent::loadResources() const {
    GameTextures::loadDust();
}
