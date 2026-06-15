#include "etempletilebuilding.h"

#include "textures/game-textures.h"

eTempleTileBuilding::eTempleTileBuilding(
        const int id, GameBoard& board,
        const eCityId cid) :
    eSanctBuilding({{0, 1, 0}}, board,
                   eBuildingType::templeTile,
                   1, 1, cid),
    mId(id) {
    setEnabled(true);
}

std::shared_ptr<eTexture>
eTempleTileBuilding::getTexture(const eTileSize size) const {
    (void)size;
    return nullptr;
}

std::shared_ptr<eTexture>
eTempleTileBuilding::getTileTexture(const eTileSize size) const {
    const int f = finished();
    if(!f) return nullptr;
    const int sizeId = static_cast<int>(size);
    const auto& blds = GameTextures::buildings()[sizeId];
    const auto& coll = blds.fSanctuaryTiles;
    if(mId > 9) return coll.getTexture(mId - 10);
    return coll.getTexture(mId);
}

std::vector<eOverlay>
eTempleTileBuilding::getOverlays(const eTileSize size) const {
    const int f = finished();
    if(!f) return {};
    if(mId < 10) return {};
    eOverlay o;
    o.fX = 0.5;
    o.fY = -0.5;
    const int sizeId = static_cast<int>(size);
    const auto& blds = GameTextures::buildings()[sizeId];
    const auto& coll = blds.fSanctuaryFire;
    o.fTex = coll.getTexture(textureTime() % coll.size());
    return {o};
}
