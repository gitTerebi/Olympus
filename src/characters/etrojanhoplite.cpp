#include "etrojanhoplite.h"

#include "textures/game-textures.h"

eTrojanHoplite::eTrojanHoplite(GameBoard& board) :
    eHopliteBase(board, &CharacterTextures::fTrojanHoplite,
                 eCharacterType::trojanHoplite) {
    GameTextures::loadTrojanHoplite();
}
