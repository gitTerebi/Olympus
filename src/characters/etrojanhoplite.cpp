#include "etrojanhoplite.h"

#include "textures/egametextures.h"

eTrojanHoplite::eTrojanHoplite(GameBoard& board) :
    eHopliteBase(board, &eCharacterTextures::fTrojanHoplite,
                 eCharacterType::trojanHoplite) {
    eGameTextures::loadTrojanHoplite();
}
