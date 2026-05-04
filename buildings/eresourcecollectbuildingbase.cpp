#include "eresourcecollectbuildingbase.h"

#include "engine/egameboard.h"
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

void eResourceCollectBuildingBase::read(eReadStream& src) {
    eResourceBuildingBase::read(src);
    src >> mNoTarget;
    if(src.formatVersion() >= eFileFormat::yearlyProduction) {
        src >> mProducedThisYear;
        for(int i = 0; i < 12; i++) src >> mMonthlyProduced[i];
        src >> mRingIdx;
    }
}

void eResourceCollectBuildingBase::write(eWriteStream& dst) const {
    eResourceBuildingBase::write(dst);
    dst << mNoTarget;
    dst << mProducedThisYear;
    for(int i = 0; i < 12; i++) dst << mMonthlyProduced[i];
    dst << mRingIdx;
}

void eResourceCollectBuildingBase::setNoTarget(const bool t) {
    mNoTarget = t;
}
