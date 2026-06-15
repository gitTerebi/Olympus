#include "eresourcecollectbuildingbase.h"

#include "engine/game-board.h"
#include "fileIO/save-archive.h"

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

void eResourceCollectBuildingBase::serializeFields(SaveArchive& ar) {
    eResourceBuildingBase::serializeFields(ar);
    ar.field("noTarget", mNoTarget);
    ar.field("producedThisYear", mProducedThisYear);
    for(int i = 0; i < 12; i++) {
        ar.field(("monthlyProduced." + std::to_string(i)).c_str(),
                 mMonthlyProduced[i]);
    }
    ar.field("ringIdx", mRingIdx);
}

void eResourceCollectBuildingBase::setNoTarget(const bool t) {
    mNoTarget = t;
}
