#include "etradeopenupevent.h"

#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "estringhelpers.h"
#include "elanguage.h"

eTradeOpenUpEvent::eTradeOpenUpEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::tradeOpensUp,
               branch, board),
    eCityEventValue(board) {}

void eTradeOpenUpEvent::trigger() {
    chooseCity();
    const auto city = this->city();
    if(!city) return;
    city->setTradeShutdown(false);
    const auto board = gameBoard();
    if(!board) return;
    eEventData ed((ePlayerCityTarget()));
    ed.fCity = city;
    board->event(eEvent::tradeOpensUp, ed);
}

std::string eTradeOpenUpEvent::longName() const {
    return eLanguage::zeusText(35, 3);
}

void eTradeOpenUpEvent::write(eWriteStream &dst) const {
    eGameEvent::write(dst);
    eCityEventValue::write(dst);
}

void eTradeOpenUpEvent::read(eReadStream &src) {
    eGameEvent::read(src);
    eCityEventValue::read(src, *gameBoard());
}
