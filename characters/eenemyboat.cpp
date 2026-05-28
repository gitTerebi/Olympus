#include "eenemyboat.h"

#include "textures/egametextures.h"

eEnemyBoat::eEnemyBoat(GameBoard& board) :
    eBoatBase(board, &eCharacterTextures::fEnemyBoat,
              eCharacterType::enemyBoat) {
    eGameTextures::loadEnemyBoat();
}
