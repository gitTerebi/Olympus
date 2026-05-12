#include "eresourcecollectbuildingbase.h"

#include "engine/e-game-board.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

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

void eResourceCollectBuildingBase::serializeJson(eJsonArchive& ar) {
    eResourceBuildingBase::serializeJson(ar);
    ar.field("mNoTarget", mNoTarget);
    ar.field("mProducedThisYear", mProducedThisYear);
    for(int i = 0; i < 12; i++) {
        const auto k = "mMonthlyProduced." + std::to_string(i);
        ar.field(k.c_str(), mMonthlyProduced[i]);
    }
    ar.field("mRingIdx", mRingIdx);
}

void eResourceCollectBuildingBase::read(eReadStream& src) {
    eResourceBuildingBase::read(src);
}

void eResourceCollectBuildingBase::write(eWriteStream& dst) const {
    eResourceBuildingBase::write(dst);
}

void eResourceCollectBuildingBase::setNoTarget(const bool t) {
    mNoTarget = t;
}
