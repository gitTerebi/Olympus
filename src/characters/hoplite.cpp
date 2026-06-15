#include "hoplite.h"

#include "textures/game-textures.h"

Hoplite::Hoplite(GameBoard& board) :
    eHopliteBase(board, &CharacterTextures::fHoplite,
                 eCharacterType::hoplite) {
    GameTextures::loadHoplite();
}
