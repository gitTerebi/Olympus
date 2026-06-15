#include "eprocessingbuilding.h"

#include "textures/game-textures.h"
#include "engine/game-board.h"
#include "fileIO/esavearchive.h"

#include <algorithm>
#include <cmath>

eProcessingBuilding::eProcessingBuilding(
        GameBoard& board,
        const eBaseTex baseTex,
        const double overlayX,
        const double overlayY,
        const eOverlays overlays,
        const eBuildingType type,
        const int sw, const int sh,
        const int maxEmployees,
        const eResourceType rawMaterial,
        const eResourceType product,
        const int rawUse,
        const int time,
        const eCityId cid) :
    eResourceBuildingBase(board, type, sw, sh,
                          maxEmployees, product, cid),
    mTextures(GameTextures::buildings()),
    mBaseTex(baseTex), mOverlays(overlays),
    mOverlayX(overlayX), mOverlayY(overlayY),
    mRawMaterial(rawMaterial), mRawUse(rawUse),
    mProcessWaitTime(time) {
    setOverlayEnabledFunc([this]() {
        return enabled() && mRawCount > 0;
    });
}

eProcessingBuilding::~eProcessingBuilding() {}

std::shared_ptr<eTexture> eProcessingBuilding::getTexture(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    return mTextures[sizeId].*mBaseTex;
}

std::vector<eOverlay> eProcessingBuilding::getOverlays(
        const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& coll = mTextures[sizeId].*mOverlays;
    const int texId = textureTime() % coll.size();
    eOverlay o;
    o.fTex = coll.getTexture(texId);
    o.fX = mOverlayX;
    o.fY = mOverlayY;
    return std::vector<eOverlay>({o});
}

void eProcessingBuilding::timeChanged(const int by) {
    if(enabled()) {
        mProcessTime += by*effectiveness();
        if(mProcessTime > mProcessWaitTime) {
            mProcessTime -= mProcessWaitTime;
            if(mRawCount >= mRawUse) {
                const auto type = resourceType();
                const int c = addProduced(type, 1);
                mRawCount -= c*mRawUse;
                mProducedThisYear += c;
                mMonthlyProduced[mRingIdx] += c;
            }
        }
    }
    eResourceBuildingBase::timeChanged(by);
}

void eProcessingBuilding::nextMonth() {
    // rolling 12-month window: each month we shift the ring buffer
    // and subtract the month that falls out of the 12-month window
    mRingIdx = (mRingIdx + 1) % 12;
    mProducedThisYear -= mMonthlyProduced[mRingIdx];
    mMonthlyProduced[mRingIdx] = 0;
    if(mProducedThisYear < 0) mProducedThisYear = 0;
}

int eProcessingBuilding::count(const eResourceType type) const {
    if(type == mRawMaterial) return mRawCount;
    return eResourceBuildingBase::count(type);
}

int eProcessingBuilding::add(const eResourceType type, const int count) {
    if(type == mRawMaterial) {
        const int r = std::clamp(mRawCount + count, 0, mMaxRaw);
        const int result = r - mRawCount;
        mRawCount = r;
        return result;
    }
    return eResourceBuildingBase::add(type, count);
}

int eProcessingBuilding::spaceLeft(const eResourceType type) const {
    if(type == mRawMaterial) return mMaxRaw - mRawCount;
    return eResourceBuildingBase::spaceLeft(type);
}

std::vector<eCartTask> eProcessingBuilding::cartTasks() const {
    auto tasks = eResourceBuildingBase::cartTasks();

    return tasks;
}

void eProcessingBuilding::serializeFields(eSaveArchive& ar) {
    eResourceBuildingBase::serializeFields(ar);
    ar.field("rawCount", mRawCount);
    ar.field("processTime", mProcessTime);
    ar.field("producedThisYear", mProducedThisYear);
    ar.field("lastMonth", mLastMonth);
    for(int i = 0; i < 12; i++) {
        ar.field(("monthlyProduced." + std::to_string(i)).c_str(),
                 mMonthlyProduced[i]);
    }
}

int eProcessingBuilding::productionPercent() const {
    return std::min(100, (int)std::round(mProcessTime / mProcessWaitTime * 100.0));
}
