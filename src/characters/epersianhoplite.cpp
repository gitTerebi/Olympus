#include "epersianhoplite.h"

#include "textures/egametextures.h"

ePersianHoplite::ePersianHoplite(GameBoard& board) :
    eHopliteBase(board, &eCharacterTextures::fPersianHoplite,
                 eCharacterType::persianHoplite) {
    eGameTextures::loadPersianHoplite();
}
