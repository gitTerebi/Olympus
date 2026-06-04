#include "esanctbuilding.h"

#include "sanctuary.h"
#include "engine/etile.h"
#include "fileIO/esavearchive.h"

bool sanctuaryTempleFrontFacing(const int rotateId, const eWorldDirection dir)
{
    static const bool table[4][4] = {
        // N      W      S      E
        { true,  true,  false, false }, // rot 0
        { true,  false, false, true  }, // rot 1
        { false, false, true,  true  }, // rot 2
        { false, true,  true,  false }, // rot 3
    };
    return table[rotateId % 4][static_cast<int>(dir)];
}

int sanctuaryFigureDirId(const int rotateId, const eWorldDirection dir)
{
    const int base = (dir == eWorldDirection::E) ? kNE :
                     (dir == eWorldDirection::N) ? kSE :
                     (dir == eWorldDirection::W) ? kSW : kNW;
    return (base + rotateId) % 4;
}

int sanctuaryTempleDirId(const int rotateId, const eWorldDirection dir)
{
    // temple: SW=0 SE=1 NE=2 NW=3
    const int base = (dir == eWorldDirection::N) ? 1 :
                     (dir == eWorldDirection::W) ? 0 :
                     (dir == eWorldDirection::S) ? 3 : 2; // E
    return (base - rotateId + 4) % 4;
}

eSanctCost totalCost(const std::vector<eSanctCost>& cost) {
    eSanctCost result{0, 0, 0};
    for(const auto& c : cost) {
        result += c;
    }
    return result;
}

eSanctBuilding::eSanctBuilding(const std::vector<eSanctCost>& cost,
                               GameBoard& board,
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

void eSanctBuilding::serializeFields(eSaveArchive& ar) {
    eBuilding::serializeFields(ar);
    ar.field("workedOn", mWorkedOn);
    ar.field("progress", mProgress);
    ar.field("halted", mHalted);
    ar.buildingAsField("monument", &getBoard(), mMonument);
    const stdptr<eSanctBuilding> tptr(this);
    ar.addPostFunc([tptr]() {
        if(!tptr) return;
        tptr->updateNextCost();
        const auto mon = tptr->monument();
        if(!mon) return;
        mon->registerElement(tptr->ref<eSanctBuilding>());
    }, "eSanctBuilding::monument");
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
