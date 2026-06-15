#include "earrowmissile.h"

#include "textures/game-textures.h"

eArrowMissile::eArrowMissile(GameBoard& board,
                             const std::vector<ePathPoint>& path) :
    eArrowSpearBase(board, &DestructionTextures::fArrow,
                    eMissileType::arrow, path) {
    GameTextures::loadArrow();
}
