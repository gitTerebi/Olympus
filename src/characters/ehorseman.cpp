#include "ehorseman.h"

#include "textures/game-textures.h"

eHorseman::eHorseman(GameBoard& board) :
    eHorsemanBase(board, &CharacterTextures::fHorseman,
                  eCharacterType::horseman) {
    GameTextures::loadHorseman();
}
