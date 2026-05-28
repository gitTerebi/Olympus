#include "emayanhoplite.h"

#include "textures/egametextures.h"

eMayanHoplite::eMayanHoplite(GameBoard& board) :
    eHopliteBase(board, &eCharacterTextures::fMayanHoplite,
                 eCharacterType::mayanHoplite) {
    eGameTextures::loadMayanHoplite();
}
