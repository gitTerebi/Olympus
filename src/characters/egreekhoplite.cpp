#include "egreekhoplite.h"

#include "textures/game-textures.h"

eGreekHoplite::eGreekHoplite(GameBoard& board) :
    eHopliteBase(board, &CharacterTextures::fGreekHoplite,
                 eCharacterType::greekHoplite) {
    GameTextures::loadGreekHoplite();
}
