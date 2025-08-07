#include "epyramidelement.h"

#include "epyramid.h"
#include "textures/egametextures.h"
#include "engine/egameboard.h"

std::vector<eSanctCost> addElevationCost(ePyramid* const pyramid,
                                         std::vector<eSanctCost> result,
                                         const int elevation) {
    for(int e = 0; e < elevation; e++) {
        const bool isDark = pyramid->darkLevel(e);
        auto cost = eSanctCost{0, 1, 0};
        if(isDark) cost.switchMarble();
        result.insert(result.begin(), cost);
        result.insert(result.begin(), eSanctCost{0, 0, 0});
        result.insert(result.begin(), cost);
        result.insert(result.begin(), eSanctCost{0, 0, 0});
    }
    return result;
}

ePyramidElement::ePyramidElement(const std::vector<eSanctCost>& cost,
                                 eGameBoard& board,
                                 const eBuildingType type,
                                 const int elevation,
                                 const int dim,
                                 const eCityId cid) :
    eSanctBuilding(cost, board, type, 1, 1, cid),
    mElevation(elevation), mDim(dim) {}

ePyramidElement::ePyramidElement(ePyramid* const pyramid,
                                 const std::vector<eSanctCost>& cost,
                                 eGameBoard& board,
                                 const eBuildingType type,
                                 const int elevation,
                                 const int dim,
                                 const eCityId cid) :
    ePyramidElement(addElevationCost(pyramid, cost, elevation),
                    board, type, elevation, dim, cid) {}

eTextureSpace ePyramidElement::getTextureSpace(
        const int tx, const int ty,
        const eTileSize size) const {
    int p = progress();
    if(p > 4*mElevation) {
        auto result = eSanctBuilding::getTextureSpace(tx, ty, size);
        result.fClamp = false;
        if(mDim == 6) {
            result.fX = -2.5;
            result.fY = 2.5;
        } else if(mDim == 5) {
            result.fX = -2.0;
            result.fY = 2.0;
        } else if(mDim == 4) {
            result.fX = -1.5;
            result.fY = 1.5;
        } else if(mDim == 2) {
            result.fX = -0.5;
            result.fY = 0.5;
        }
        return result;
    }
    const SDL_Point pt{tx, ty};
    const auto& r = tileRect();
    if(!SDL_PointInRect(&pt, &r)) return {nullptr};
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings()[sizeId];
    if(mCurrentElevation == 0 && p == 0) {
        const auto& coll = blds.fSanctuarySpace;
        auto& board = getBoard();
        const auto tile = board.tile(tx, ty);
        const int seed = tile->seed();
        eTextureSpace result;
        result.fRect = {tx, ty, 1, 1};
        result.fTex = coll.getTexture(seed % coll.size());
        return result;
    }
    const auto m = monument();
    const auto py = static_cast<ePyramid*>(m);
    const auto& texs = blds.fPyramid;
    if(mCurrentElevation >= mElevation) {
        const bool isDark = py->darkLevel(mCurrentElevation - 1);
        eTextureSpace result;
        result.fRect = {tx, ty, 1, 1};
        int texId = 39 - 1;
        if(!halted()) {
            texId = 39 - 1;
            result.fX = 0;
            result.fY = 0;
        } else {
            texId = 38 - 1;
            result.fX = 2.0;
            result.fY = 2.0;
        }
        if(isDark) texId += 5;
        result.fTex = texs.getTexture(texId);
        return result;
    }
    p -= 4*mCurrentElevation;
    if(r.x == tx && r.y == ty) {
        eTextureSpace result;
        result.fRect = {tx, ty, 1, 1};
        int texId = 39 - 1;
        const bool isDark = p <= 0 ? py->darkLevel(mCurrentElevation - 1) :
                                     py->darkLevel(mCurrentElevation);
        if(p <= 0) {
            if(!halted()) {
                texId = 39 - 1;
                result.fX = 0;
                result.fY = 0;
            } else {
                texId = 38 - 1;
                result.fX = 2.0;
                result.fY = 2.0;
            }
        } else if(p == 1) {
            texId = 35 - 1;
            result.fX = 0.5;
            result.fY = 0.5;
        } else if(p == 2) {
            texId = 36 - 1;
            result.fX = 1.0;
            result.fY = 1.0;
        } else if(p == 3) {
            texId = 37 - 1;
            result.fX = 1.5;
            result.fY = 1.5;
        } else {
            texId = 38 - 1;
            result.fX = 2.0;
            result.fY = 2.0;
        }
        if(isDark) texId += 5;
        result.fTex = texs.getTexture(texId);
        return result;
    }
    return {nullptr};
}

void ePyramidElement::progressed() {
    mCurrentElevation = mElevation;
    int p = progress();
    const auto& rect = tileRect();
    auto& board = getBoard();
    for(int e = 0; e < mElevation; e++) {
        if(p <= 4 && p >= 1) {
            const int tx = rect.x;
            const int ty = rect.y;
            const auto tile = board.tile(tx, ty);
            const int da = tile->doubleAltitude();
            tile->setDoubleAltitude(da + 1, false);
        }
        if(p < 4) {
            mCurrentElevation = e;
            return;
        }
        p -= 4;
    }
}

void ePyramidElement::read(eReadStream& src) {
    eSanctBuilding::read(src);

    src >> mCurrentElevation;
}

void ePyramidElement::write(eWriteStream& dst) const {
    eSanctBuilding::write(dst);

    dst << mCurrentElevation;
}
