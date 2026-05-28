#include "epersianhorseman.h"

#include "textures/egametextures.h"

ePersianHorseman::ePersianHorseman(GameBoard& board) :
    eHorsemanBase(board, &eCharacterTextures::fPersianHorseman,
                  eCharacterType::persianHorseman) {
    eGameTextures::loadPersianHorseman();
}
