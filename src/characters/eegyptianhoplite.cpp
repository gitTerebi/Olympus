#include "eegyptianhoplite.h"

#include "textures/egametextures.h"

eEgyptianHoplite::eEgyptianHoplite(GameBoard& board) :
    eHopliteBase(board, &eCharacterTextures::fEgyptianHoplite,
                 eCharacterType::egyptianHoplite) {
    eGameTextures::loadEgyptianHoplite();
}
