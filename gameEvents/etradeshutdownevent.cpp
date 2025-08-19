#include "etradeshutdownevent.h"

#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "estringhelpers.h"
#include "elanguage.h"

eTradeShutDownEvent::eTradeShutDownEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::tradeShutdowns,
                 branch, board) {}

void eTradeShutDownEvent::trigger() {
    const auto city = this->city();
    if(!city) return;
    city->setTradeShutdown(true);
    const auto board = gameBoard();
    if(!board) return;
    eEventData ed((ePlayerCityTarget()));
    ed.fCity = city;
    board->event(eEvent::tradeShutdowns, ed);
}

std::string eTradeShutDownEvent::longName() const {
    auto tmpl = eLanguage::text("trade_shutdown_long_name");
    const auto none = eLanguage::text("none");
    const auto city = this->city();
    const auto ctstr = city ? city->name() : none;
    eStringHelpers::replace(tmpl, "%1", ctstr);
    return tmpl;
}

void eTradeShutDownEvent::write(eWriteStream &dst) const {
    eGameEvent::write(dst);
    eCityEvent::write(dst);
}

void eTradeShutDownEvent::read(eReadStream &src) {
    eGameEvent::read(src);
    eCityEvent::read(src, *gameBoard());
}
