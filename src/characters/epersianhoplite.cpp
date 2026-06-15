#include "epersianhoplite.h"

#include "textures/game-textures.h"

ePersianHoplite::ePersianHoplite(GameBoard& board) :
    eHopliteBase(board, &CharacterTextures::fPersianHoplite,
                 eCharacterType::persianHoplite) {
    GameTextures::loadPersianHoplite();
}
