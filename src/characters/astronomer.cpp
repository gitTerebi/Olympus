#include "astronomer.h"

#include "textures/game-textures.h"

Astronomer::Astronomer(GameBoard& board) :
    eBasicPatroler(board, &CharacterTextures::fAstronomer,
                   eCharacterType::astronomer) {
    GameTextures::loadAstronomer();
    setProvide(eProvide::actorAstronomer, 10000);
}
