#include "epyramidtemple.h"

#include "textures/game-textures.h"
#include "epyramid.h"

ePyramidTemple::ePyramidTemple(const std::vector<eSanctCost>& cost,
                               GameBoard& board,
                               const int elevation,
                               const eCityId cid) :
    ePyramidElement(cost, board, eBuildingType::pyramidTemple,
                    elevation, 4, cid) {
    GameTextures::loadSanctuary();
}

ePyramidTemple::ePyramidTemple(ePyramid* const pyramid,
                               GameBoard& board,
                               const int elevation,
                               const eCityId cid) :
    ePyramidElement(pyramid,
                    {{2, 2, 0}, {1, 1, 0}, {2, 2, 0}},
                    board, eBuildingType::pyramidTemple,
                    elevation, 4, cid) {
    GameTextures::loadSanctuary();
}

stdsptr<eTexture> ePyramidTemple::getTexture(const eTileSize size) const {
    const int p = 3 - maxProgress() + progress();
    if(p <= 0) return nullptr;
    const int sizeId = static_cast<int>(size);
    const auto& blds = GameTextures::buildings()[sizeId];
    const int id = p - 1;
    const int dirId = 0;
    if(atlantean() && id == 2) {
        GameTextures::loadPoseidonSanctuary();
        const auto& coll = blds.fPoseidonSanctuary;
        return coll.getTexture(dirId);
    } else {
        if(id == 2) {
            GameTextures::loadZeusSanctuary();
        }
        const auto& coll = blds.fSanctuary[dirId];
        return coll.getTexture(id);
    }
}
