#include "ehorseman.h"

#include "textures/egametextures.h"

eHorseman::eHorseman(GameBoard& board) :
    eHorsemanBase(board, &eCharacterTextures::fHorseman,
                  eCharacterType::horseman) {
    eGameTextures::loadHorseman();
}
