#include "erivalarmyawayevent.h"

#include "engine/game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "language.h"
#include "fileIO/save-archive.h"

eRivalArmyAwayEvent::eRivalArmyAwayEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        GameBoard& board) :
    eGameEvent(cid, eGameEventType::rivalArmyAway,
               branch, board),
    eCityEventValue(board) {}

void eRivalArmyAwayEvent::trigger() {
    chooseCity();
    if(!mCity) return;
    const auto board = gameBoard();
    if(!board) return;
    eEventData ed((ePlayerCityTarget()));
    ed.fCity = mCity;
    const int str = mCity->militaryStrength();
    mCity->setMilitaryStrength(str - 1);
    board->event(eEvent::rivalArmyAway, ed);
}

std::string eRivalArmyAwayEvent::longName() const {
    return Language::text("rival_army_away_long_name");
}

void eRivalArmyAwayEvent::serializeFields(SaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    eCityEventValue::serialize(ar, *gameBoard());
}
