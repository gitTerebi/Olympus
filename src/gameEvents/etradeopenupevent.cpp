#include "etradeopenupevent.h"
#include "fileIO/save-archive.h"

#include "engine/game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "string-helpers.h"
#include "language.h"

eTradeOpenUpEvent::eTradeOpenUpEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        GameBoard& board) :
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
    return Language::zeusText(35, 3);
}

void eTradeOpenUpEvent::serializeFields(SaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    eCityEventValue::serialize(ar, *gameBoard());
}
