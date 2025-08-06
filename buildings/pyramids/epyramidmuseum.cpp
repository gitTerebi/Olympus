#include "epyramidmuseum.h"

#include "textures/egametextures.h"
#include "epyramid.h"

ePyramidMuseum::ePyramidMuseum(
        const std::vector<eSanctCost>& cost,
        eGameBoard& board,
        const int elevation,
        const eCityId cid) :
    ePyramidElement(cost, board, eBuildingType::pyramidMuseum,
                    6, 6, elevation, cid) {
    eGameTextures::loadMuseum();
}

ePyramidMuseum::ePyramidMuseum(
        ePyramid* const pyramid,
        eGameBoard& board,
        const int elevation,
        const eCityId cid) :
    ePyramidElement(pyramid,
                    {eSanctCost{10, 10, 0}},
                    board, eBuildingType::pyramidMuseum,
                    6, 6, elevation, cid) {
    eGameTextures::loadMuseum();
}

stdsptr<eTexture> ePyramidMuseum::getTexture(const eTileSize size) const {
    if(!finished()) return nullptr;
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings()[sizeId];
    return blds.fMuseum;
}
