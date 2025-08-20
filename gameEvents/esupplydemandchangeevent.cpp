#include "esupplydemandchangeevent.h"

eSupplyDemandChangeEvent::eSupplyDemandChangeEvent(
    const eCityId cid, const eGameEventType type,
    const eGameEventBranch branch, eGameBoard &board) :
    eGameEvent(cid, type, branch, board),
    eCityEventValue(board) {}

void eSupplyDemandChangeEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eResourceEventValue::write(dst);
    eCountEventValue::write(dst);
    eCityEventValue::write(dst);
}

void eSupplyDemandChangeEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    eResourceEventValue::read(src);
    eCountEventValue::read(src);
    eCityEventValue::read(src, *gameBoard());
}
