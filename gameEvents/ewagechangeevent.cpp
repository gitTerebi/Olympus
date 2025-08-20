#include "ewagechangeevent.h"

#include "engine/egameboard.h"
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
}

void eWageChangeEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    eCountEventValue::read(src);
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
