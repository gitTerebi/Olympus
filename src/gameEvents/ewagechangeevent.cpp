#include "ewagechangeevent.h"
#include "fileIO/save-archive.h"

#include "engine/game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "language.h"

eWageChangeEvent::eWageChangeEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        GameBoard& board) :
    eGameEvent(cid, eGameEventType::wageChange,
               branch, board) {}

void eWageChangeEvent::serializeFields(SaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    eCountEventValue::serialize(ar);
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
    return Language::text("wage_change");
}
