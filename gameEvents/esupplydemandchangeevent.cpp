#include "esupplydemandchangeevent.h"

void eSupplyDemandChangeEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eResourceEvent::write(dst);
    eCountEvent::write(dst);
    eCityEvent::write(dst);
}

void eSupplyDemandChangeEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    eResourceEvent::read(src);
    eCountEvent::read(src);
    eCityEvent::read(src, *gameBoard());
}
