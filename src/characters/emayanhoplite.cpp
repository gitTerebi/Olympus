#include "emayanhoplite.h"

#include "textures/game-textures.h"

eMayanHoplite::eMayanHoplite(GameBoard& board) :
    eHopliteBase(board, &CharacterTextures::fMayanHoplite,
                 eCharacterType::mayanHoplite) {
    GameTextures::loadMayanHoplite();
}
