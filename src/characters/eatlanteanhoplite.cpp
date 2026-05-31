#include "eatlanteanhoplite.h"

#include "textures/egametextures.h"

eAtlanteanHoplite::eAtlanteanHoplite(GameBoard& board) :
    eHopliteBase(board, &eCharacterTextures::fAtlanteanHoplite,
                 eCharacterType::atlanteanHoplite) {
    eGameTextures::loadAtlanteanHoplite();
}
