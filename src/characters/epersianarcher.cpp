#include "epersianarcher.h"

#include "textures/egametextures.h"

ePersianArcher::ePersianArcher(GameBoard& board) :
    eArcherBase(board, &eCharacterTextures::fPersianArcher,
                eCharacterType::persianArcher) {
    eGameTextures::loadPersianArcher();
}
