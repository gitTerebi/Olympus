#include "epyramidtop.h"

#include "textures/game-textures.h"
#include "epyramid.h"

ePyramidTop::ePyramidTop(const std::vector<eSanctCost>& cost,
                         GameBoard& board,
                         const int elevation,
                         const eCityId cid) :
    ePyramidElement(cost, board, eBuildingType::pyramidTop,
                    elevation, 1, cid) {}

ePyramidTop::ePyramidTop(ePyramid* const pyramid,
                         GameBoard& board,
                         const int elevation,
                         const eCityId cid) :
    ePyramidElement(pyramid,
                    {pyramid->swapMarbleIfDark(elevation, eSanctCost{0, 2, 0})},
                    board, eBuildingType::pyramidTop,
                    elevation, 1, cid) {}

stdsptr<Texture> ePyramidTop::getTexture(const eTileSize size) const {
    if(!finished()) return nullptr;
    const int sizeId = static_cast<int>(size);
    const auto& blds = GameTextures::buildings()[sizeId];
    const int e = elevation();
    const auto m = monument();
    const auto p = static_cast<ePyramid*>(m);
    const bool isDark = p->darkLevel(e);
    const int texId = isDark ? 26 : 9;
    return blds.fPyramid.getTexture(texId - 1);
}
