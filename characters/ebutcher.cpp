#include "ebutcher.h"

#include "textures/egametextures.h"

eButcher::eButcher(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fButcher,
                   eCharacterType::butcher) {
    eGameTextures::loadButcher();
}
