#include "elavamissile.h"

eLavaMissile::eLavaMissile(eGameBoard& board,
                           const std::vector<ePathPoint>& path) :
    eMissile(board, eMissileType::lava, path) {
    eGameTextures::loadLava();
    setSpeed(0.02);
}

std::shared_ptr<eTexture>
eLavaMissile::getTexture(const eTileSize size) const {
    const int id = static_cast<int>(size);
    const auto& textures = eGameTextures::destrution();
    const auto& coll = textures[id].fLava;
    const int texId = textureTime();
    return coll.getTexture(texId % coll.size());
}
