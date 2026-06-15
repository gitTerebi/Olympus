#include "arrow-missile.h"

#include "textures/game-textures.h"

ArrowMissile::ArrowMissile(GameBoard& board,
                             const std::vector<PathPoint>& path) :
    ArrowSpearBase(board, &DestructionTextures::fArrow,
                    MissileType::arrow, path) {
    GameTextures::loadArrow();
}
