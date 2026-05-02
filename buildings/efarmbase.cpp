#include "efarmbase.h"

#include "textures/egametextures.h"
#include "enumbers.h"

#include <algorithm>

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
        else if(i < mCurrentTile) texId = 4;
        else if(i == mCurrentTile) texId = mCurrentStage;
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
            if(++mCurrentStage >= 5) {
                mCurrentStage = 0;
                if(++mCurrentTile >= 5) {
                    addProduced(resourceType(), 4);
                    mCurrentTile = 0;
                }
            }
        }
    }
    eResourceBuildingBase::timeChanged(by);
}

void eFarmBase::read(eReadStream& src) {
    eResourceBuildingBase::read(src);

    src >> mNextRipe;
    int combined;
    src >> combined;
    mCurrentTile  = std::clamp(combined / 5, 0, 4);
    mCurrentStage = std::clamp(combined % 5, 0, 4);
}

void eFarmBase::write(eWriteStream& dst) const {
    eResourceBuildingBase::write(dst);

    dst << mNextRipe;
    dst << (mCurrentTile * 5 + mCurrentStage);
}
