#include "ephoenicianhorseman.h"

#include "textures/egametextures.h"

ePhoenicianHorseman::ePhoenicianHorseman(GameBoard& board) :
    eHorsemanBase(board, &eCharacterTextures::fPhoenicianHorseman,
                  eCharacterType::phoenicianHorseman) {
    eGameTextures::loadPhoenicianHorseman();
}
