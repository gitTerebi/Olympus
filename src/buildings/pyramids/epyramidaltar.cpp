#include "epyramidaltar.h"

#include "textures/game-textures.h"
#include "epyramid.h"

ePyramidAltar::ePyramidAltar(const std::vector<eSanctCost>& cost,
                             GameBoard& board,
                             const int elevation,
                             const eCityId cid) :
    ePyramidElement(cost, board, eBuildingType::pyramidAltar,
                    elevation, 2, cid) {
    GameTextures::loadSanctuary();
}

ePyramidAltar::ePyramidAltar(ePyramid* const pyramid,
                             GameBoard& board,
                             const int elevation,
                             const eCityId cid) :
    ePyramidElement(pyramid,
                    {eSanctCost{0, 4, 0}},
                    board, eBuildingType::pyramidAltar,
                    elevation, 2, cid) {
    GameTextures::loadSanctuary();
}

stdsptr<eTexture> ePyramidAltar::getTexture(const eTileSize size) const {
    if(!finished()) return nullptr;
    const int sizeId = static_cast<int>(size);
    const auto& blds = GameTextures::buildings()[sizeId];
    return blds.fSanctuaryAltar;
}
