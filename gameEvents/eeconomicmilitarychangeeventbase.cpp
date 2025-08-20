#include "eeconomicmilitarychangeeventbase.h"

eEconomicMilitaryChangeEventBase::eEconomicMilitaryChangeEventBase(
    const eCityId cid, const eGameEventType type,
    const eGameEventBranch branch, eGameBoard &board) :
    eGameEvent(cid, type, branch, board),
    eCityEventValue(board) {}

void eEconomicMilitaryChangeEventBase::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eCountEventValue::write(dst);
    eCityEventValue::write(dst);
}

void eEconomicMilitaryChangeEventBase::read(eReadStream& src) {
    eGameEvent::read(src);
    eCountEventValue::read(src);
    eCityEventValue::read(src, *gameBoard());
}
