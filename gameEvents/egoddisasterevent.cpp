#include "egoddisasterevent.h"

#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "elanguage.h"

eGodDisasterEvent::eGodDisasterEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::godDisaster,
               branch, board) {}

void eGodDisasterEvent::trigger() {
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
        e->setCity(mCity);
        e->setEnd(true);
        const auto date = board->date();
        e->initializeDate(date + mDuration);
        addConsequence(e);
        board->event(eEvent::godDisaster, ed);
    }
}

std::string eGodDisasterEvent::longName() const {
    return eLanguage::zeusText(35, 13);
}

void eGodDisasterEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eGodEventValue::write(dst);
    eCityEvent::write(dst);
    dst << mDuration;
    dst << mEnd;
}

void eGodDisasterEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    eGodEventValue::read(src);
    eCityEvent::read(src, *gameBoard());
    src >> mDuration;
    src >> mEnd;
}
