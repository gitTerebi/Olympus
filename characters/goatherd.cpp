#include "goatherd.h"

#include "textures/egametextures.h"

Goatherd::Goatherd(GameBoard& board) :
    eShepherdBase(board, &eCharacterTextures::fGoatherd,
                  eCharacterType::goatherd) {
    eGameTextures::loadGoatherd();
}
