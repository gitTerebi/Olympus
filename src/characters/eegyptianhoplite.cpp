#include "eegyptianhoplite.h"

#include "textures/game-textures.h"

eEgyptianHoplite::eEgyptianHoplite(GameBoard& board) :
    eHopliteBase(board, &CharacterTextures::fEgyptianHoplite,
                 eCharacterType::egyptianHoplite) {
    GameTextures::loadEgyptianHoplite();
}
