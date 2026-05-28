#include "eshepherd.h"

#include "textures/egametextures.h"

eShepherd::eShepherd(GameBoard& board) :
    eShepherdBase(board, &eCharacterTextures::fShepherd,
                  eCharacterType::shepherd) {
    eGameTextures::loadShepherd();
}
