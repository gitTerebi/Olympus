#include "shepherd.h"

#include "textures/egametextures.h"

Shepherd::Shepherd(GameBoard& board) :
    eShepherdBase(board, &eCharacterTextures::fShepherd,
                  eCharacterType::shepherd) {
    eGameTextures::loadShepherd();
}
