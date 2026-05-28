#include "eoceanidhoplite.h"

#include "textures/egametextures.h"

eOceanidHoplite::eOceanidHoplite(GameBoard& board) :
    eHopliteBase(board, &eCharacterTextures::fOceanidHoplite,
                 eCharacterType::oceanidHoplite) {
    eGameTextures::loadOceanidHoplite();
}
