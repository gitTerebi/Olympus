#include "epersianhorseman.h"

#include "textures/game-textures.h"

ePersianHorseman::ePersianHorseman(GameBoard& board) :
    eHorsemanBase(board, &CharacterTextures::fPersianHorseman,
                  eCharacterType::persianHorseman) {
    GameTextures::loadPersianHorseman();
}
