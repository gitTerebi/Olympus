#include "eeconomicmilitarychangeeventbase.h"

#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

eEconomicMilitaryChangeEventBase::eEconomicMilitaryChangeEventBase(
    const eCityId cid, const eGameEventType type,
    const eGameEventBranch branch, eGameBoard &board) :
    eGameEvent(cid, type, branch, board),
    eCityEventValue(board) {}

void eEconomicMilitaryChangeEventBase::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eSaveArchive ar(dst);
    const_cast<eEconomicMilitaryChangeEventBase*>(this)->serialize(ar);
}

void eEconomicMilitaryChangeEventBase::read(eReadStream& src) {
    eGameEvent::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eEconomicMilitaryChangeEventBase::serialize(eSaveArchive& ar) {
    if(ar.reading()) {
        eCountEventValue::read(ar.readStream());
        eCityEventValue::read(ar.readStream(), *gameBoard());
    } else {
        eCountEventValue::write(ar.writeStream());
        eCityEventValue::write(ar.writeStream());
    }
}

void eEconomicMilitaryChangeEventBase::serializeJson(eJsonArchive& ar) {
    eGameEvent::serializeJson(ar);
    eCountEventValue::serializeJson(ar);
    eCityEventValue::serializeJson(ar, *gameBoard());
}
