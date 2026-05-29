#include "astronomer.h"

#include "textures/egametextures.h"

Astronomer::Astronomer(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fAstronomer,
                   eCharacterType::astronomer) {
    eGameTextures::loadAstronomer();
    setProvide(eProvide::actorAstronomer, 10000);
}
