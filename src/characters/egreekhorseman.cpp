#include "egreekhorseman.h"

#include "textures/game-textures.h"

eGreekHorseman::eGreekHorseman(GameBoard& board) :
    eHorsemanBase(board, &CharacterTextures::fGreekHorseman,
                  eCharacterType::greekHorseman) {
    GameTextures::loadGreekHorseman();
}
