#include "egreekhoplite.h"

#include "textures/egametextures.h"

eGreekHoplite::eGreekHoplite(GameBoard& board) :
    eHopliteBase(board, &eCharacterTextures::fGreekHoplite,
                 eCharacterType::greekHoplite) {
    eGameTextures::loadGreekHoplite();
}
