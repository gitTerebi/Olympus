#include "eeconomicmilitarychangeeventbase.h"

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
