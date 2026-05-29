#include "butcher.h"

#include "textures/egametextures.h"

Butcher::Butcher(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fButcher,
                   eCharacterType::butcher) {
    eGameTextures::loadButcher();
}
