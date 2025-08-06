#include "epyramidelement.h"

#include "epyramid.h"
#include "textures/egametextures.h"
#include "engine/egameboard.h"

std::vector<eSanctCost> addElevationCost(std::vector<eSanctCost> result,
                                         const int sw, const int sh,
                                         const int elevation) {
    for(int e = 0; e < elevation; e++) {
        for(int x = 0; x < sw; x++) {
            for(int y = 0; y < sh; y++) {
                result.insert(result.begin(), eSanctCost{0, 1, 0});
                result.insert(result.begin(), eSanctCost{0, 0, 0});
                result.insert(result.begin(), eSanctCost{0, 1, 0});
                result.insert(result.begin(), eSanctCost{0, 0, 0});
            }
        }
    }
    return result;
}

ePyramidElement::ePyramidElement(const std::vector<eSanctCost>& cost,
                                 eGameBoard& board,
                                 const eBuildingType type,
                                 const int sw, const int sh,
                                 const int elevation,
                                 const eCityId cid) :
    eSanctBuilding(addElevationCost(cost, sw, sh, elevation),
                   board, type, sw, sh, cid),
    mElevation(elevation) {}

eTextureSpace ePyramidElement::getTextureSpace(
        const int tx, const int ty,
        const eTileSize size) const {
    const int sw = spanW();
    const int sh = spanH();
    int p = progress();
    if(p > 4*mElevation*sw*sh) {
        return eSanctBuilding::getTextureSpace(tx, ty, size);
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
    p -= 4*mCurrentElevation*sw*sh;
    for(int x = 0; x < sw; x++) {
        for(int y = 0; y < sh; y++) {
            if(r.x + x == tx && r.y + y == ty) {
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
            p -= 4;
        }
    }
    return {nullptr};
}

void ePyramidElement::progressed() {
    mCurrentElevation = mElevation;
    int p = progress();
    const auto& rect = tileRect();
    auto& board = getBoard();
    const int sw = spanW();
    const int sh = spanH();
    for(int e = 0; e < mElevation; e++) {
        for(int x = 0; x < sw; x++) {
            for(int y = 0; y < sh; y++) {
                if(p <= 4 && p >= 1) {
                    const int tx = rect.x + x;
                    const int ty = rect.y + y;
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
    }
}

SDL_Rect ePyramidElement::nextElement() const {
    int p = progress();
    const auto& rect = tileRect();
    const int sw = spanW();
    const int sh = spanH();
    p -= 4*mCurrentElevation*sw*sh;
    for(int x = 0; x < sw; x++) {
        for(int y = 0; y < sh; y++) {
            if(p < 4) {
                return {rect.x + x, rect.y + y, 1, 1};
            }
            p -= 4;
        }
    }
    return rect;
}

void ePyramidElement::read(eReadStream& src) {
    eSanctBuilding::read(src);

    src >> mCurrentElevation;
}

void ePyramidElement::write(eWriteStream& dst) const {
    eSanctBuilding::write(dst);

    dst << mCurrentElevation;
}
