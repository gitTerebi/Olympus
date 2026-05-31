#include "etrojanhorseman.h"

#include "textures/egametextures.h"

eTrojanHorseman::eTrojanHorseman(GameBoard& board) :
    eHorsemanBase(board, &eCharacterTextures::fTrojanHorseman,
                  eCharacterType::trojanHorseman) {
    eGameTextures::loadTrojanHorseman();
}
