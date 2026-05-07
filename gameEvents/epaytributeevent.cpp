#include "epaytributeevent.h"

#include "engine/egameboard.h"
#include "elanguage.h"
#include "estringhelpers.h"
#include "fileIO/esavearchive.h"

ePayTributeEvent::ePayTributeEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::payTribute, branch, board) {}

void ePayTributeEvent::initialize(const stdsptr<eWorldCity>& c) {
    mCity = c;
}

void ePayTributeEvent::trigger() {
    if(!mCity) return;
    const auto board = gameBoard();
    if(!board) return;
    const auto pid = playerId();
    board->tributeFrom(pid, mCity, false);
}

std::string ePayTributeEvent::longName() const {
    auto tmpl = eLanguage::text("receive_tribute_from");
    const auto none = eLanguage::text("none");
    const auto ctstr = mCity ? mCity->name() : none;
    eStringHelpers::replace(tmpl, "%1", ctstr);
    return tmpl;
}

void ePayTributeEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eSaveArchive ar(dst);
    const_cast<ePayTributeEvent*>(this)->serialize(ar);
}

void ePayTributeEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void ePayTributeEvent::serialize(eSaveArchive& ar) {
    if(ar.reading()) {
        ar.readStream().readCity(worldBoard(), [this](const stdsptr<eWorldCity>& c) {
            mCity = c;
        });
    } else {
        ar.writeStream().writeCity(mCity.get());
    }
}
