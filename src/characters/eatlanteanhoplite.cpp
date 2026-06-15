#include "eatlanteanhoplite.h"

#include "textures/game-textures.h"

eAtlanteanHoplite::eAtlanteanHoplite(GameBoard& board) :
    eHopliteBase(board, &CharacterTextures::fAtlanteanHoplite,
                 eCharacterType::atlanteanHoplite) {
    GameTextures::loadAtlanteanHoplite();
}
