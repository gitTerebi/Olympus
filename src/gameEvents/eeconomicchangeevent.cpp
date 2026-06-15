#include "eeconomicchangeevent.h"

#include "engine/game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "language.h"

eEconomicChangeEvent::eEconomicChangeEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        GameBoard& board) :
    eEconomicMilitaryChangeEventBase(
        cid, eGameEventType::economicChange, branch, board) {}

void eEconomicChangeEvent::trigger() {
    chooseCity();
    const auto city = this->city();
    chooseCount();
    const int by = this->count();
    if(!city || by == 0) return;
    int w = city->wealth();
    w = std::clamp(w + by, 1, 5);
    city->setWealth(w);
    const auto board = gameBoard();
    if(!board) return;
    eEventData ed((ePlayerCityTarget()));
    ed.fCity = city;
    if(by > 0) board->event(eEvent::economicProsperity, ed);
    else board->event(eEvent::economicDecline, ed);
}

std::string eEconomicChangeEvent::longName() const {
    return Language::text("economic_change_long_name");
}
