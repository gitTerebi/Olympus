#include "eoceanidhoplite.h"

#include "textures/game-textures.h"

eOceanidHoplite::eOceanidHoplite(GameBoard& board) :
    eHopliteBase(board, &CharacterTextures::fOceanidHoplite,
                 eCharacterType::oceanidHoplite) {
    GameTextures::loadOceanidHoplite();
}
