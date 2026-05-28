#include "emayanarcher.h"

#include "textures/egametextures.h"

eMayanArcher::eMayanArcher(GameBoard& board) :
    eArcherBase(board, &eCharacterTextures::fMayanArcher,
                eCharacterType::mayanArcher) {
    eGameTextures::loadMayanArcher();
}
