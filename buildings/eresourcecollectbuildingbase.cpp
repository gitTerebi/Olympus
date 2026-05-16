#include "eresourcecollectbuildingbase.h"

#include "engine/e-game-board.h"
#include "fileIO/esavearchive.h"

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
    ar.field("noTarget", mNoTarget);
    ar.field("producedThisYear", mProducedThisYear);
    for(int i = 0; i < 12; i++) {
        ar.field(("monthlyProduced." + std::to_string(i)).c_str(),
                 mMonthlyProduced[i]);
    }
    ar.field("ringIdx", mRingIdx);
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
