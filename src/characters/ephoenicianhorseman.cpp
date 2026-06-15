#include "ephoenicianhorseman.h"

#include "textures/game-textures.h"

ePhoenicianHorseman::ePhoenicianHorseman(GameBoard& board) :
    eHorsemanBase(board, &CharacterTextures::fPhoenicianHorseman,
                  eCharacterType::phoenicianHorseman) {
    GameTextures::loadPhoenicianHorseman();
}
