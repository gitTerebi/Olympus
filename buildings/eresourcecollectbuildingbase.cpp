#include "eresourcecollectbuildingbase.h"

#include "engine/egameboard.h"

void eResourceCollectBuildingBase::read(eReadStream& src) {
    eResourceBuildingBase::read(src);
    src >> mNoTarget;
}

void eResourceCollectBuildingBase::write(eWriteStream& dst) const {
    eResourceBuildingBase::write(dst);
    dst << mNoTarget;
}

void eResourceCollectBuildingBase::setNoTarget(const bool t) {
    mNoTarget = t;
}
