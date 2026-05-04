#include "eprocessingbuilding.h"

#include "characters/actions/ecarttransporteraction.h"
#include "textures/egametextures.h"
#include "engine/egameboard.h"
#include "enumbers.h"

#include <algorithm>
#include <cmath>

eProcessingBuilding::eProcessingBuilding(
        eGameBoard& board,
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
    mTextures(eGameTextures::buildings()),
    mBaseTex(baseTex), mOverlays(overlays),
    mOverlayX(overlayX), mOverlayY(overlayY),
    mRawMaterial(rawMaterial), mRawUse(rawUse),
    mProcessWaitTime(time) {
    setOverlayEnabledFunc([this]() {
        return enabled() && mRawCount > 0;
    });
}

eProcessingBuilding::~eProcessingBuilding() {
    if(mTakeCart) mTakeCart->kill();
}

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
        if(!mTakeCart) {
            mTakeCart = spawnCart(eCartActionTypeSupport::take);
            mTakeCart->setMaxDistance(eNumbers::sProcessingBuildingMaxResourceTakeDistance);
        }
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

    if(mMaxRaw > mRawCount) {
        eCartTask task;
        task.fType = eCartActionType::take;
        task.fResource = mRawMaterial;
        task.fMaxCount = mMaxRaw - mRawCount;
        tasks.push_back(task);
    }

    return tasks;
}

void eProcessingBuilding::read(eReadStream& src) {
    eResourceBuildingBase::read(src);

    src.readCharacter(&getBoard(), [this](eCharacter* const c) {
        mTakeCart = static_cast<eCartTransporter*>(c);
    });

    src >> mRawCount;
    src >> mProcessTime;
    mProducedThisYear = 0;
    mLastMonth = 0;
    mMonthlyProduced.fill(0);
}

void eProcessingBuilding::write(eWriteStream& dst) const {
    eResourceBuildingBase::write(dst);

    dst.writeCharacter(mTakeCart);

    dst << mRawCount;
    dst << mProcessTime;
    dst << mProducedThisYear;
    dst << mLastMonth;
    for(int i = 0; i < 12; i++) dst << mMonthlyProduced[i];
}

int eProcessingBuilding::productionPercent() const {
    return std::min(100, (int)std::round(mProcessTime / mProcessWaitTime * 100.0));
}
