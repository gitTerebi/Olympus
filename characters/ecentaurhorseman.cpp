#include "ecentaurhorseman.h"

#include "textures/egametextures.h"

eCentaurHorseman::eCentaurHorseman(GameBoard& board) :
    eHorsemanBase(board, &eCharacterTextures::fCentaurHorseman,
                  eCharacterType::centaurHorseman) {
    eGameTextures::loadCentaurHorseman();
}
