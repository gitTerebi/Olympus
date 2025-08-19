#include "eeconomicmilitarychangeeventbase.h"

void eEconomicMilitaryChangeEventBase::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eCountEvent::write(dst);
    eCityEvent::write(dst);
}

void eEconomicMilitaryChangeEventBase::read(eReadStream& src) {
    eGameEvent::read(src);
    eCountEvent::read(src);
    eCityEvent::read(src, *gameBoard());
}
