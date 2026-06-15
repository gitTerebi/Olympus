#include "rock-missile.h"

#include "textures/game-textures.h"

RockMissile::RockMissile(GameBoard& board,
                           const std::vector<PathPoint>& path) :
    Missile(board, MissileType::rock, path) {
    GameTextures::loadRock();
}

std::shared_ptr<Texture> RockMissile::getTexture(const eTileSize size) const {
    const int id = static_cast<int>(size);
    const auto& textures = GameTextures::destrution();
    const auto& rockTex = textures[id].fRock;
    return rockTex.getTexture(0);
}
