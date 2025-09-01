#include "edustmissile.h"

eDustMissile::eDustMissile(eGameBoard& board,
                           const std::vector<ePathPoint>& path) :
    eMissile(board, eMissileType::dust, path),
    mCollId(eRand::rand()) {
    eGameTextures::loadDust();
    setSpeed(0.02);
}

std::shared_ptr<eTexture>
eDustMissile::getTexture(const eTileSize size) const {
    const int id = static_cast<int>(size);
    const auto& textures = eGameTextures::destrution();
    const auto& colls = textures[id].fDust;
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
