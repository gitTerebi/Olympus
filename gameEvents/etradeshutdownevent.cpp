#include "etradeshutdownevent.h"

#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "elanguage.h"

eTradeShutDownEvent::eTradeShutDownEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::tradeShutdowns,
              branch, board),
    eCityEventValue(board) {}

void eTradeShutDownEvent::trigger() {
    chooseCity();
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
    return eLanguage::zeusText(35, 2);
}

void eTradeShutDownEvent::write(eWriteStream &dst) const {
    eGameEvent::write(dst);
    eCityEventValue::write(dst);
}

void eTradeShutDownEvent::read(eReadStream &src) {
    eGameEvent::read(src);
    eCityEventValue::read(src, *gameBoard());
}
