#include "epyramidmuseum.h"

#include "textures/game-textures.h"
#include "epyramid.h"

ePyramidMuseum::ePyramidMuseum(
        const std::vector<eSanctCost>& cost,
        GameBoard& board,
        const int elevation,
        const eCityId cid) :
    ePyramidElement(cost, board, eBuildingType::pyramidMuseum,
                    elevation, 6, cid) {
    GameTextures::loadMuseum();
}

ePyramidMuseum::ePyramidMuseum(
        ePyramid* const pyramid,
        GameBoard& board,
        const int elevation,
        const eCityId cid) :
    ePyramidElement(pyramid,
                    {eSanctCost{10, 10, 0}},
                    board, eBuildingType::pyramidMuseum,
                    elevation, 6, cid) {
    GameTextures::loadMuseum();
}

stdsptr<Texture> ePyramidMuseum::getTexture(const eTileSize size) const {
    if(!finished()) return nullptr;
    const int sizeId = static_cast<int>(size);
    const auto& blds = GameTextures::buildings()[sizeId];
    return blds.fMuseum;
}
