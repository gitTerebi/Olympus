#include "etrojanhorseman.h"

#include "textures/game-textures.h"

eTrojanHorseman::eTrojanHorseman(GameBoard& board) :
    eHorsemanBase(board, &CharacterTextures::fTrojanHorseman,
                  eCharacterType::trojanHorseman) {
    GameTextures::loadTrojanHorseman();
}
