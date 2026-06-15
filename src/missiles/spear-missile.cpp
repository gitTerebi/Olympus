#include "spear-missile.h"

#include "textures/game-textures.h"

SpearMissile::SpearMissile(GameBoard& board,
                             const std::vector<PathPoint>& path) :
    ArrowSpearBase(board, &DestructionTextures::fSpear,
                    MissileType::spear, path) {
    GameTextures::loadSpear();
}
