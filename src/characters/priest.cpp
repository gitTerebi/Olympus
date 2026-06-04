#include "priest.h"

#include "textures/echaractertextures.h"
#include "textures/egametextures.h"

Priest::Priest(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fPriest,
                   eCharacterType::priest) {
    eGameTextures::loadPriest();
}
