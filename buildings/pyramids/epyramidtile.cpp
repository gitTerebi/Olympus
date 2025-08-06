#include "epyramidtile.h"

#include "textures/egametextures.h"
#include "epyramid.h"

ePyramidTile::ePyramidTile(const std::vector<eSanctCost>& cost,
                           eGameBoard& board,
                           const int elevation,
                           const int type,
                           const eCityId cid) :
    ePyramidElement(cost, board, eBuildingType::pyramidTile,
                    1, 1, elevation, cid),
    mType(type) {
    eGameTextures::loadPalaceTiles();
}

ePyramidTile::ePyramidTile(ePyramid* const pyramid,
                           eGameBoard& board,
                           const int elevation,
                           const int type,
                           const eCityId cid) :
    ePyramidElement(pyramid,
                    {pyramid->swapMarbleIfDark(elevation, eSanctCost{0, 1, 0})},
                    board, eBuildingType::pyramidTile,
                    1, 1, elevation, cid),
    mType(type) {
    eGameTextures::loadPalaceTiles();
}

stdsptr<eTexture> ePyramidTile::getTexture(const eTileSize size) const {
    if(!finished()) return nullptr;
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings()[sizeId];
    if(mType == 0) {
        const int e = elevation() - 1;
        const auto m = monument();
        const auto p = static_cast<ePyramid*>(m);
        const bool isDark = e < 0 ? false : p->darkLevel(e);
        if(isDark) return blds.fPyramid2.getTexture(33 - 1);
        else blds.fPalaceTiles.getTexture(0);
    } else if(mType == 1) {
        return blds.fPyramid2.getTexture(34 - 1);
    } else if(mType == 2) {
        return blds.fPyramid2.getTexture(35 - 1);
    }
    return nullptr;
}
