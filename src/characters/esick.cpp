#include "esick.h"

#include "textures/egametextures.h"

eSick::eSick(GameBoard& board) :
    eFightingPatroler(board, &eCharacterTextures::fSick,
                      eCharacterType::sick) {
    eGameTextures::loadSick();
    setSpeed(18.375);
}
