#include "epyramidtemple.h"

#include "textures/egametextures.h"
#include "epyramid.h"

ePyramidTemple::ePyramidTemple(const std::vector<eSanctCost>& cost,
                               eGameBoard& board,
                               const int elevation,
                               const eCityId cid) :
    ePyramidElement(cost, board, eBuildingType::pyramidTemple,
                    4, 4, elevation, cid) {
    eGameTextures::loadSanctuary();
}

ePyramidTemple::ePyramidTemple(ePyramid* const pyramid,
                               eGameBoard& board,
                               const int elevation,
                               const eCityId cid) :
    ePyramidElement(pyramid,
                    {{2, 2, 0}, {1, 1, 0}, {2, 2, 0}},
                    board, eBuildingType::pyramidTemple,
                    4, 4, elevation, cid) {
    eGameTextures::loadSanctuary();
}

stdsptr<eTexture> ePyramidTemple::getTexture(const eTileSize size) const {
    const int p = 3 - maxProgress() + progress();
    if(p <= 0) return nullptr;
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings()[sizeId];
    const int id = p - 1;
    const int dirId = 0;
    if(atlantean() && id == 2) {
        eGameTextures::loadPoseidonSanctuary();
        const auto& coll = blds.fPoseidonSanctuary;
        return coll.getTexture(dirId);
    } else {
        if(id == 2) {
            eGameTextures::loadZeusSanctuary();
        }
        const auto& coll = blds.fSanctuary[dirId];
        return coll.getTexture(id);
    }
}
