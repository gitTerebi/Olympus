#include "escholar.h"

#include "textures/egametextures.h"

eScholar::eScholar(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fScholar,
                   eCharacterType::scholar) {
    eGameTextures::loadScholar();
    setProvide(eProvide::gymnastScholar, 10000);
}
