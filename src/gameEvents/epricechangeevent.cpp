#include "epricechangeevent.h"
#include "fileIO/save-archive.h"

#include "engine/game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "language.h"

ePriceChangeEvent::ePriceChangeEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        GameBoard& board) :
    eGameEvent(cid, eGameEventType::priceChange, branch, board) {}

void ePriceChangeEvent::trigger() {
    const auto board = gameBoard();
    if(!board) return;
    chooseType();
    chooseCount();
    board->incPrice(mResource, mCount);
    eEventData ed((ePlayerCityTarget()));
    ed.fResourceType = resourceType();
    const auto e = count() > 0 ? eEvent::priceIncrease :
                                         eEvent::priceDecrease;
    board->event(e, ed);
}

std::string ePriceChangeEvent::longName() const {
    auto tmpl = Language::text("price_change_long_name");
    longNameReplaceResource("%1", tmpl);
    return tmpl;
}

void ePriceChangeEvent::serializeFields(SaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    eResourceEventValue::serialize(ar);
    eCountEventValue::serialize(ar);
}
