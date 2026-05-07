#include "eresourcecollectbuildingbase.h"

#include "engine/egameboard.h"
#include "fileIO/esavearchive.h"
#include "fileIO/efileformat.h"

void eResourceCollectBuildingBase::nextMonth() {
    mRingIdx = (mRingIdx + 1) % 12;
    mProducedThisYear -= mMonthlyProduced[mRingIdx];
    mMonthlyProduced[mRingIdx] = 0;
    if(mProducedThisYear < 0) mProducedThisYear = 0;
}

void eResourceCollectBuildingBase::trackProduced(const int c) {
    mProducedThisYear += c;
    mMonthlyProduced[mRingIdx] += c;
}

void eResourceCollectBuildingBase::serialize(eSaveArchive& ar) {
    ar.value(mNoTarget);
    if(ar.versionAtLeast(eFileFormat::cartTarget)) {
        ar.value(mProducedThisYear);
        for(int i = 0; i < 12; i++) ar.value(mMonthlyProduced[i]);
        ar.value(mRingIdx);
    }
}

void eResourceCollectBuildingBase::read(eReadStream& src) {
    eResourceBuildingBase::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eResourceCollectBuildingBase::write(eWriteStream& dst) const {
    eResourceBuildingBase::write(dst);
    eSaveArchive ar(dst);
    const_cast<eResourceCollectBuildingBase*>(this)->serialize(ar);
}

void eResourceCollectBuildingBase::setNoTarget(const bool t) {
    mNoTarget = t;
}
