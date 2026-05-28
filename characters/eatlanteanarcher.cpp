#include "eatlanteanarcher.h"

#include "textures/egametextures.h"

eAtlanteanArcher::eAtlanteanArcher(GameBoard& board) :
    eArcherBase(board, &eCharacterTextures::fAtlanteanArcher,
                eCharacterType::atlanteanArcher) {
    eGameTextures::loadAtlanteanArcher();
}
