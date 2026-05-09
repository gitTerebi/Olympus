#include "ereceivetributeevent.h"
#include "engine/e-game-board.h"
#include "engine/e-tribute.h"
#include "elanguage.h"
#include "estringhelpers.h"
#include "fileIO/esavearchive.h"

eReceiveTributeEvent::eReceiveTributeEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::receiveTribute, branch, board) {}

void eReceiveTributeEvent::initialize(const stdsptr<eWorldCity>& c) {
    mCity = c;
}

void eReceiveTributeEvent::trigger() {
    if(!mCity) return;
    const auto board = gameBoard();
    if(!board) return;
    const auto pid = playerId();
    eTributeHelpers::receiveTributeFromCity(*board, pid, mCity, false);
}

std::string eReceiveTributeEvent::longName() const {
    auto tmpl = eLanguage::text("receive_tribute_from");
    const auto none = eLanguage::text("none");
    const auto ctstr = mCity ? mCity->name() : none;
    eStringHelpers::replace(tmpl, "%1", ctstr);
    return tmpl;
}

void eReceiveTributeEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eSaveArchive ar(dst);
    const_cast<eReceiveTributeEvent*>(this)->serialize(ar);
}

void eReceiveTributeEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eReceiveTributeEvent::serialize(eSaveArchive& ar) {
    if(ar.reading()) {
        ar.readStream().readCity(worldBoard(), [this](const stdsptr<eWorldCity>& c) {
            mCity = c;
        });
    } else {
        ar.writeStream().writeCity(mCity.get());
    }
}
