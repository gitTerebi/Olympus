#include "elavamissile.h"

eLavaMissile::eLavaMissile(eGameBoard& board,
                           const std::vector<ePathPoint>& path) :
    eMissile(board, eMissileType::lava, path),
    mCollId(eRand::rand()) {
    eGameTextures::loadLava();
    setSpeed(0.02);
}

std::shared_ptr<eTexture>
eLavaMissile::getTexture(const eTileSize size) const {
    const int id = static_cast<int>(size);
    const auto& textures = eGameTextures::destrution();
    const auto& colls = textures[id].fLava;
    const int collId = mCollId % colls.size();
    const auto& coll = colls[collId];
    const int texId = textureTime() + mTexTimeShift;
    const int clampedTexId = std::clamp(texId, 0, coll.size() - 1);
    if(clampedTexId == coll.size() - 1) {
        mCollId = eRand::rand();
        mTexTimeShift -= texId;
    }
    return coll.getTexture(clampedTexId);
}
