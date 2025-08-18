#include "ewavemissile.h"

eWaveMissile::eWaveMissile(eGameBoard& board,
                           const std::vector<ePathPoint>& path) :
    eMissile(board, eMissileType::wave, path) {
    eGameTextures::loadWave();
    setSpeed(0.1);
}

std::shared_ptr<eTexture>
eWaveMissile::getTexture(const eTileSize size) const {
    const int id = static_cast<int>(size);
    const auto& textures = eGameTextures::destrution();
    const auto& coll = textures[id].fWave;
    const int texId = textureTime();
    return coll.getTexture(texId % coll.size());
}
