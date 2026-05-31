#include "hoplite.h"

#include "textures/egametextures.h"

Hoplite::Hoplite(GameBoard& board) :
    eHopliteBase(board, &eCharacterTextures::fHoplite,
                 eCharacterType::hoplite) {
    eGameTextures::loadHoplite();
}
