#include "esupplydemandchangeevent.h"

#include "fileIO/esavearchive.h"

eSupplyDemandChangeEvent::eSupplyDemandChangeEvent(
    const eCityId cid, const eGameEventType type,
    const eGameEventBranch branch, eGameBoard &board) :
    eGameEvent(cid, type, branch, board),
    eCityEventValue(board) {}

void eSupplyDemandChangeEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eSaveArchive ar(dst);
    const_cast<eSupplyDemandChangeEvent*>(this)->serialize(ar);
}

void eSupplyDemandChangeEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eSupplyDemandChangeEvent::serialize(eSaveArchive& ar) {
    if(ar.reading()) {
        eResourceEventValue::read(ar.readStream());
        eCountEventValue::read(ar.readStream());
        eCityEventValue::read(ar.readStream(), *gameBoard());
    } else {
        eResourceEventValue::write(ar.writeStream());
        eCountEventValue::write(ar.writeStream());
        eCityEventValue::write(ar.writeStream());
    }
}
