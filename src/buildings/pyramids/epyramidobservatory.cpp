#include "epyramidobservatory.h"

#include "textures/game-textures.h"
#include "epyramid.h"

ePyramidObservatory::ePyramidObservatory(
        const std::vector<eSanctCost>& cost,
        GameBoard& board,
        const int elevation,
        const eCityId cid) :
    ePyramidElement(cost, board, eBuildingType::pyramidObservatory,
                    elevation, 5, cid) {
    GameTextures::loadObservatory();
}

ePyramidObservatory::ePyramidObservatory(
        ePyramid* const pyramid,
        GameBoard& board,
        const int elevation,
        const eCityId cid) :
    ePyramidElement(pyramid,
                    {eSanctCost{5, 5, 0}},
                    board, eBuildingType::pyramidObservatory,
                    elevation, 5, cid) {
    GameTextures::loadObservatory();
}

stdsptr<eTexture> ePyramidObservatory::getTexture(const eTileSize size) const {
    if(!finished()) return nullptr;
    const int sizeId = static_cast<int>(size);
    const auto& blds = GameTextures::buildings()[sizeId];
    return blds.fObservatory;
}
