#include "esanctbuilding.h"

#include "esanctuary.h"

eSanctCost totalCost(const std::vector<eSanctCost>& cost) {
    eSanctCost result{0, 0, 0};
    for(const auto& c : cost) {
        result += c;
    }
    return result;
}

eSanctBuilding::eSanctBuilding(const std::vector<eSanctCost>& cost,
                               eGameBoard& board,
                               const eBuildingType type,
                               const int sw, const int sh,
                               const eCityId cid) :
    eBuilding(board, type, sw, sh, cid),
    mMaxProgress(cost.size()),
    mCost(cost),
    mTotalCost(totalCost(cost)) {
    setOverlayEnabledFunc([]() { return true; });
    updateNextCost();
}

void eSanctBuilding::erase() {
    mMonument->erase();
}

bool eSanctBuilding::resourcesAvailable() const {
    if(finished()) return false;
    if(mHalted) return false;
    const auto& s = mMonument->stored();
    if(s.fMarble < mNextCost.fMarble) return false;
    if(s.fWood < mNextCost.fWood) return false;
    if(s.fSculpture < mNextCost.fSculpture) return false;
    if(s.fBlackMarble < mNextCost.fBlackMarble) return false;
    if(s.fOrichalc < mNextCost.fOrichalc) return false;
    return true;
}

bool eSanctBuilding::finished() const {
    return mProgress >= mMaxProgress;
}

bool eSanctBuilding::incProgress() {
    if(mProgress >= mMaxProgress) return false;
    const auto& cost = mCost[mProgress++];
    mMonument->useResources(cost);
    updateNextCost();
    scheduleTerrainUpdate();
    progressed();
    mMonument->buildingProgressed();
    return true;
}

void eSanctBuilding::destroy() {
    mProgress = 0;
    scheduleTerrainUpdate();
}

void eSanctBuilding::setMonument(eMonument* const s) {
    mMonument = s;
}

void eSanctBuilding::read(eReadStream& src) {
    eBuilding::read(src);

    src >> mWorkedOn;
    src >> mProgress;
    src >> mHalted;
    updateNextCost();
}

void eSanctBuilding::write(eWriteStream& dst) const {
    eBuilding::write(dst);

    dst << mWorkedOn;
    dst << mProgress;
    dst << mHalted;
}

void eSanctBuilding::scheduleTerrainUpdate() {
    const auto bt = type();
    if(bt == eBuildingType::templeTile) {
        const auto t = centerTile();
        if(t) t->scheduleTerrainUpdate();
    }
}

void eSanctBuilding::updateNextCost() {
    if(mProgress >= mMaxProgress) {
        mNextCost = {0, 0, 0};
    } else {
        mNextCost = mCost[mProgress];
    }
}
