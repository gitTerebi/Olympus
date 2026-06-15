#include "egoddisasterevent.h"

#include "engine/game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "language.h"
#include "fileIO/save-archive.h"

eGodDisasterEvent::eGodDisasterEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        GameBoard& board) :
    eGameEvent(cid, eGameEventType::godDisaster,
               branch, board),
    eCityEventValue(board) {}

void eGodDisasterEvent::trigger() {
    chooseCity();
    if(!mCity) return;
    const auto board = gameBoard();
    if(!board) return;
    const auto cid = cityId();
    eEventData ed(cid);
    ed.fCity = mCity;
    ed.fGod = mGod;
    if(mEnd) {
        mCity->setTradeShutdown(false);
        board->event(eEvent::godDisasterEnds, ed);
    } else {
        mCity->setTradeShutdown(true);
        const auto e = e::make_shared<eGodDisasterEvent>(
                           cid, eGameEventBranch::child, *board);
        e->setGod(mGod);
        e->setSingleCity(mCity);
        e->setEnd(true);
        const auto date = board->date();
        e->initializeDate(date + mDuration);
        addConsequence(e);
        board->event(eEvent::godDisaster, ed);
    }
}

std::string eGodDisasterEvent::longName() const {
    return Language::zeusText(35, 13);
}

void eGodDisasterEvent::serializeFields(SaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    eGodEventValue::serialize(ar);
    eCityEventValue::serialize(ar, *gameBoard());
    ar.field("duration", mDuration, 180);
    ar.field("end", mEnd, false);
}
