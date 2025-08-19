#include "esupplydemandchangeevent.h"

void eSupplyDemandChangeEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eResourceEvent::write(dst);
    eCityEvent::write(dst);
    dst << mBy;
}

void eSupplyDemandChangeEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    eResourceEvent::read(src);
    eCityEvent::read(src, *gameBoard());
    src >> mBy;
}
