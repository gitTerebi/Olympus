#include "ehoplite.h"

#include "textures/egametextures.h"

eHoplite::eHoplite(GameBoard& board) :
    eHopliteBase(board, &eCharacterTextures::fHoplite,
                 eCharacterType::hoplite) {
    eGameTextures::loadHoplite();
}
