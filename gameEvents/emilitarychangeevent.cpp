#include "emilitarychangeevent.h"

#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "elanguage.h"

eMilitaryChangeEvent::eMilitaryChangeEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eEconomicMilitaryChangeEventBase(cid,
        eGameEventType::militaryChange, branch, board) {}

void eMilitaryChangeEvent::trigger() {
    chooseCity();
    const auto city = this->city();
    chooseCount();
    const int by = this->count();
    if(!city || by == 0) return;
    int s = city->militaryStrength();
    s = std::clamp(s + by, 1, 6);
    city->setMilitaryStrength(s);
    const auto board = gameBoard();
    if(!board) return;
    eEventData ed((ePlayerCityTarget()));
    ed.fCity = city;
    if(by > 0) board->event(eEvent::militaryBuildup, ed);
    else board->event(eEvent::militaryDecline, ed);
}

std::string eMilitaryChangeEvent::longName() const {
    return eLanguage::text("military_change_long_name");
}
