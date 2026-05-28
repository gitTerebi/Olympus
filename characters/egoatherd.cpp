#include "egoatherd.h"

#include "textures/egametextures.h"

eGoatherd::eGoatherd(GameBoard& board) :
    eShepherdBase(board, &eCharacterTextures::fGoatherd,
                  eCharacterType::goatherd) {
    eGameTextures::loadGoatherd();
}
