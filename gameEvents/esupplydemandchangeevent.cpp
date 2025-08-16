#include "esupplydemandchangeevent.h"

void eSupplyDemandChangeEvent::write(eWriteStream& dst) const {
    eResourceCityEvent::write(dst);
    dst << mBy;
}

void eSupplyDemandChangeEvent::read(eReadStream& src) {
    eResourceCityEvent::read(src);
    src >> mBy;
}
