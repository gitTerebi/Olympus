#include "erockmissile.h"

#include "textures/game-textures.h"

eRockMissile::eRockMissile(GameBoard& board,
                           const std::vector<ePathPoint>& path) :
    eMissile(board, eMissileType::rock, path) {
    GameTextures::loadRock();
}

std::shared_ptr<eTexture> eRockMissile::getTexture(const eTileSize size) const {
    const int id = static_cast<int>(size);
    const auto& textures = GameTextures::destrution();
    const auto& rockTex = textures[id].fRock;
    return rockTex.getTexture(0);
}
