#include "dust-missile.h"

DustMissile::DustMissile(GameBoard& board,
                           const std::vector<PathPoint>& path) :
    Missile(board, MissileType::dust, path),
    mCollId(Rand::rand()) {
    GameTextures::loadDust();
    setSpeed(0.02);
}

std::shared_ptr<Texture>
DustMissile::getTexture(const eTileSize size) const {
    const int id = static_cast<int>(size);
    const auto& textures = GameTextures::destrution();
    const auto& colls = textures[id].fDust;
    const int collId = mCollId % colls.size();
    const auto& coll = colls[collId];
    const int texId = textureTime();
    const int clampedTexId = std::clamp(texId, 0, coll.size() - 1);
    return coll.getTexture(clampedTexId);
}
