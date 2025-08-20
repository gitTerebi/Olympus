#include "epricechangeevent.h"

#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "elanguage.h"

ePriceChangeEvent::ePriceChangeEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
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
    auto tmpl = eLanguage::text("price_change_long_name");
    longNameReplaceResource("%1", tmpl);
    return tmpl;
}

void ePriceChangeEvent::write(eWriteStream &dst) const {
    eGameEvent::write(dst);
    eResourceEventValue::write(dst);
    eCountEventValue::write(dst);
}

void ePriceChangeEvent::read(eReadStream &src) {
    eGameEvent::read(src);
    eResourceEventValue::read(src);
    eCountEventValue::read(src);
}
