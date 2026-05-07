#include "egoddisasterevent.h"

#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "elanguage.h"
#include "fileIO/esavearchive.h"

eGodDisasterEvent::eGodDisasterEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
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
    return eLanguage::zeusText(35, 13);
}

void eGodDisasterEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eSaveArchive ar(dst);
    const_cast<eGodDisasterEvent*>(this)->serialize(ar);
}

void eGodDisasterEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eGodDisasterEvent::serialize(eSaveArchive& ar) {
    if(ar.reading()) {
        eGodEventValue::read(ar.readStream());
        eCityEventValue::read(ar.readStream(), *gameBoard());
    } else {
        eGodEventValue::write(ar.writeStream());
        eCityEventValue::write(ar.writeStream());
    }
    ar.value(mDuration);
    ar.value(mEnd);
}
