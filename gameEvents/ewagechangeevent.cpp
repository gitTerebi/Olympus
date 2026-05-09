#include "ewagechangeevent.h"
#include "fileIO/esavearchive.h"

#include "engine/e-game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "elanguage.h"

eWageChangeEvent::eWageChangeEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::wageChange,
               branch, board) {}

void eWageChangeEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eCountEventValue::write(dst);
    eSaveArchive ar(dst);
    const_cast<eWageChangeEvent*>(this)->serialize(ar);
}

void eWageChangeEvent::serialize(eSaveArchive& ar) {

}

void eWageChangeEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    eCountEventValue::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eWageChangeEvent::trigger() {
    const auto board = gameBoard();
    if(!board) return;
    chooseCount();
    const int c = count();
    board->changeWage(c);
    eEventData ed((ePlayerCityTarget()));
    const auto e = c > 0 ? eEvent::wageIncrease :
                           eEvent::wageDecrease;
    board->event(e, ed);
}

std::string eWageChangeEvent::longName() const {
    return eLanguage::text("wage_change");
}
