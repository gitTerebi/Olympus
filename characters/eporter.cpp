#include "eporter.h"

#include "textures/egametextures.h"

ePorter::ePorter(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fPorter,
                   eCharacterType::porter) {
    eGameTextures::loadPorter();
}
