#include "wave-missile.h"

WaveMissile::WaveMissile(GameBoard& board,
                           const std::vector<PathPoint>& path) :
    Missile(board, MissileType::wave, path) {
    GameTextures::loadWave();
    setSpeed(0.1);
}

std::shared_ptr<Texture>
WaveMissile::getTexture(const eTileSize size) const {
    const int id = static_cast<int>(size);
    const auto& textures = GameTextures::destrution();
    const auto& coll = textures[id].fWave;
    const int texId = textureTime();
    return coll.getTexture(texId % coll.size());
}
