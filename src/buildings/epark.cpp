#include "epark.h"

#include "textures/game-textures.h"
#include "textures/varying-size-tex.h"
#include "textures/park-texture.h"

ePark::ePark(GameBoard& board,
             const eCityId cid) :
    eBuilding(board, eBuildingType::park, 1, 1, cid),
    mTextures(GameTextures::buildings()) {
    GameTextures::loadPark();
}

std::shared_ptr<eTexture> ePark::getTexture(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& sizeColl = mTextures[sizeId];
    return sizeColl.fPark.getTexture(0);
}
