#include "etradeshutdownevent.h"
#include "fileIO/esavearchive.h"

#include "engine/game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "elanguage.h"

eTradeShutDownEvent::eTradeShutDownEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        GameBoard& board) :
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

void eTradeShutDownEvent::serializeFields(eSaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    eCityEventValue::serialize(ar, *gameBoard());
}
