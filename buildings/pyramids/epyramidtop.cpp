#include "epyramidtop.h"

#include "textures/egametextures.h"
#include "epyramid.h"

ePyramidTop::ePyramidTop(const std::vector<eSanctCost>& cost,
                         eGameBoard& board,
                         const int elevation,
                         const eCityId cid) :
    ePyramidElement(cost, board, eBuildingType::pyramidTop,
                    1, 1, elevation, cid) {}

ePyramidTop::ePyramidTop(ePyramid* const pyramid,
                         eGameBoard& board,
                         const int elevation,
                         const eCityId cid) :
    ePyramidElement(pyramid,
                    {pyramid->swapMarbleIfDark(elevation, eSanctCost{0, 2, 0})},
                    board, eBuildingType::pyramidTop,
                    1, 1, elevation, cid) {}

stdsptr<eTexture> ePyramidTop::getTexture(const eTileSize size) const {
    if(!finished()) return nullptr;
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings()[sizeId];
    const int e = elevation();
    const auto m = monument();
    const auto p = static_cast<ePyramid*>(m);
    const bool isDark = p->darkLevel(e);
    const int texId = isDark ? 26 : 9;
    return blds.fPyramid.getTexture(texId - 1);
}
