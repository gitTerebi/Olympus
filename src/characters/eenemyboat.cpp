#include "eenemyboat.h"

#include "textures/game-textures.h"

eEnemyBoat::eEnemyBoat(GameBoard& board) :
    eBoatBase(board, &CharacterTextures::fEnemyBoat,
              eCharacterType::enemyBoat) {
    GameTextures::loadEnemyBoat();
}
