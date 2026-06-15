#include "lava-missile.h"

LavaMissile::LavaMissile(GameBoard& board,
                           const std::vector<PathPoint>& path) :
    Missile(board, MissileType::lava, path),
    mCollId(Rand::rand()) {
    GameTextures::loadLava();
    setSpeed(0.02);
}

std::shared_ptr<Texture>
LavaMissile::getTexture(const eTileSize size) const {
    const int id = static_cast<int>(size);
    const auto& textures = GameTextures::destrution();
    const auto& colls = textures[id].fLava;
    const int collId = mCollId % colls.size();
    const auto& coll = colls[collId];
    const int texId = textureTime() + mTexTimeShift;
    const int clampedTexId = std::clamp(texId, 0, coll.size() - 1);
    if(clampedTexId == coll.size() - 1) {
        mCollId = Rand::rand();
        mTexTimeShift -= texId;
    }
    return coll.getTexture(clampedTexId);
}
