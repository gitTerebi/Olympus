#include "epark.h"

#include "textures/egametextures.h"
#include "textures/evaryingsizetex.h"
#include "textures/eparktexture.h"

ePark::ePark(eGameBoard& board,
             const eCityId cid) :
    eBuilding(board, eBuildingType::park, 1, 1, cid),
    mTextures(eGameTextures::buildings()) {
    eGameTextures::loadPark();
}

std::shared_ptr<eTexture> ePark::getTexture(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& sizeColl = mTextures[sizeId];
    return sizeColl.fPark.getTexture(0);
}
