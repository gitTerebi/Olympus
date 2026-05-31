#include "egreekhorseman.h"

#include "textures/egametextures.h"

eGreekHorseman::eGreekHorseman(GameBoard& board) :
    eHorsemanBase(board, &eCharacterTextures::fGreekHorseman,
                  eCharacterType::greekHorseman) {
    eGameTextures::loadGreekHorseman();
}
