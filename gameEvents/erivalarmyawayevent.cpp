#include "erivalarmyawayevent.h"

#include "engine/e-game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "elanguage.h"
#include "fileIO/esavearchive.h"

eRivalArmyAwayEvent::eRivalArmyAwayEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
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
    return eLanguage::text("rival_army_away_long_name");
}

void eRivalArmyAwayEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eSaveArchive ar(dst);
    const_cast<eRivalArmyAwayEvent*>(this)->serialize(ar);
}

void eRivalArmyAwayEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eRivalArmyAwayEvent::serialize(eSaveArchive& ar) {
    if(ar.reading()) {
        eCityEventValue::read(ar.readStream(), *gameBoard());
    } else {
        eCityEventValue::write(ar.writeStream());
    }
}
