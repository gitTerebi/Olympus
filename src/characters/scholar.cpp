#include "scholar.h"

#include "textures/egametextures.h"

Scholar::Scholar(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fScholar,
                   eCharacterType::scholar) {
    eGameTextures::loadScholar();
    setProvide(eProvide::gymnastScholar, 10000);
}
