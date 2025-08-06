#include "epyramidtop.h"

#include "textures/egametextures.h"

ePyramidTop::ePyramidTop(eGameBoard& board,
                         const int elevation,
                         const eCityId cid) :
    ePyramidElement({{0, 2, 0}}, board, eBuildingType::pyramidTop,
                    1, 1, elevation, cid) {}

stdsptr<eTexture> ePyramidTop::getTexture(const eTileSize size) const {
    if(!finished()) return nullptr;
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings()[sizeId];
    const int e = elevation();
    const bool isDark = e % 2;
    const int texId = isDark ? 26 : 9;
    return blds.fPyramid.getTexture(texId - 1);
}
