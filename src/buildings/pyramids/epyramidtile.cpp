#include "epyramidtile.h"

#include "textures/game-textures.h"
#include "epyramid.h"

ePyramidTile::ePyramidTile(const std::vector<eSanctCost>& cost,
                           GameBoard& board,
                           const int elevation,
                           const int type,
                           const eCityId cid) :
    ePyramidElement(cost, board, eBuildingType::pyramidTile,
                    elevation, 1, cid),
    mType(type) {
    GameTextures::loadPalaceTiles();
}

ePyramidTile::ePyramidTile(ePyramid* const pyramid,
                           GameBoard& board,
                           const int elevation,
                           const int type,
                           const eCityId cid) :
    ePyramidElement(pyramid,
                    {pyramid->swapMarbleIfDark(elevation, eSanctCost{0, 1, 0})},
                    board, eBuildingType::pyramidTile,
                    elevation, 1, cid),
    mType(type) {
    GameTextures::loadPalaceTiles();
}

stdsptr<Texture> ePyramidTile::getTexture(const eTileSize size) const {
    if(!finished()) return nullptr;
    const int sizeId = static_cast<int>(size);
    const auto& blds = GameTextures::buildings()[sizeId];
    if(mType == 0) {
        const int e = elevation() - 1;
        const auto m = monument();
        const auto p = static_cast<ePyramid*>(m);
        const bool isDark = e < 0 ? false : p->darkLevel(e);
        if(isDark) return blds.fPyramid2.getTexture(33 - 1);
        else return blds.fPalaceTiles.getTexture(0);
    } else if(mType == 1) {
        return blds.fPyramid2.getTexture(34 - 1);
    } else if(mType == 2) {
        return blds.fPyramid2.getTexture(35 - 1);
    }
    return nullptr;
}
