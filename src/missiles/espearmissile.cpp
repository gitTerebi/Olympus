#include "espearmissile.h"

#include "textures/game-textures.h"

eSpearMissile::eSpearMissile(GameBoard& board,
                             const std::vector<ePathPoint>& path) :
    eArrowSpearBase(board, &DestructionTextures::fSpear,
                    eMissileType::spear, path) {
    GameTextures::loadSpear();
}
