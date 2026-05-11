#include "efarmbase.h"

#include "textures/egametextures.h"
#include "enumbers.h"
#include "engine/edate.h"
#include "engine/e-game-board.h"
#include "fileIO/esavearchive.h"

#include <algorithm>
#include <cmath>

eFarmBase::eFarmBase(eGameBoard& board,
                     const eBuildingType type,
                     const int sw, const int sh,
                     const eResourceType resType,
                     const eCityId cid) :
    eResourceBuildingBase(board, type, sw, sh, 10, resType, cid),
    mTextures(eGameTextures::buildings())  {
    eGameTextures::loadPlantation();
}

std::shared_ptr<eTexture> eFarmBase::getTexture(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    return mTextures[sizeId].fPlantation;
}

std::vector<eOverlay> eFarmBase::getOverlays(const eTileSize size) const {
    std::vector<eOverlay> os;
    const int sizeId = static_cast<int>(size);
    const auto& texs = mTextures[sizeId];
    const std::pair<int, int> xy[5] = {{-1, -1},
                                       {0, -1},
                                       {1, -1},
                                       {1, -2},
                                       {1, -3}};
    const double e = effectiveness();
    const int usedFields = std::clamp((int)std::round(1 + e*4), 0, 5);
    for(int i = 0; i < 5; i++) {
        eOverlay& o = os.emplace_back();
        const auto& xxyy = xy[i];
        o.fX = xxyy.first;
        o.fY = xxyy.second;
        o.fAlignTop = true;
        int texId;
        if(i >= usedFields) texId = 0;
        else if(i < mGrownFields) texId = 4;
        else if(i == mGrownFields) texId = mFieldStage;
        else texId = 0;
        const auto type = resourceType();
        switch(type) {
        case eResourceType::onions:
            o.fTex = texs.fOnions.getTexture(texId);
            break;
        case eResourceType::carrots:
            o.fTex = texs.fCarrots.getTexture(texId);
            break;
        case eResourceType::wheat:
            o.fTex = texs.fWheat.getTexture(texId);
            break;
        default: break;
        }
    }
    return os;
}

void eFarmBase::timeChanged(const int by) {
    if(enabled()) {
        mNextRipe += by*effectiveness();
        if(mNextRipe > eNumbers::sFarmRipePeriod / 5.0) {
            mNextRipe = 0;
            if(++mFieldStage >= 5) {
                mFieldStage = 0;
                if(mGrownFields < 5) ++mGrownFields;
            }
        }
    }
    eResourceBuildingBase::timeChanged(by);
}

void eFarmBase::nextMonth() {
    mRingIdx = (mRingIdx + 1) % 12;
    mProducedThisYear -= mMonthlyProduced[mRingIdx];
    mMonthlyProduced[mRingIdx] = 0;
    if(mProducedThisYear < 0) mProducedThisYear = 0;

    if(getBoard().date().month() == eMonth::july) {
        // harvest whatever has grown this year, then reset
        const int steps = mGrownFields * 5 + mFieldStage;
        const int c = addProduced(resourceType(), steps * 8 / 25);
        mProducedThisYear += c;
        mMonthlyProduced[mRingIdx] += c;
        mNextRipe = 0;
        mGrownFields = 0;
        mFieldStage = 0;
    }
}

int eFarmBase::productionPercent() const {
    const int currentStep = mGrownFields * 5 + mFieldStage;
    return currentStep * 100 / 25;
}

eMonth eFarmBase::nextHarvestMonth() const {
    return eMonth::july;
}

void eFarmBase::serialize(eSaveArchive& ar) {
    ar.field("mNextRipe", mNextRipe);
    int combined;
    if(ar.writing()) combined = mGrownFields * 5 + mFieldStage;
    ar.field("combined", combined);
    if(ar.reading()) {
        mGrownFields  = std::clamp(combined / 5, 0, 5);
        mFieldStage = std::clamp(combined % 5, 0, 4);
    }
    ar.field("mProducedThisYear", mProducedThisYear);
    for(int i = 0; i < 12; i++) ar.field("mMonthlyProduced[i]", mMonthlyProduced[i]);
    ar.field("mRingIdx", mRingIdx);
}

void eFarmBase::read(eReadStream& src) {
    eResourceBuildingBase::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eFarmBase::write(eWriteStream& dst) const {
    eResourceBuildingBase::write(dst);
    eSaveArchive ar(dst);
    const_cast<eFarmBase*>(this)->serialize(ar);
}
