#include "edisgruntled.h"

#include "textures/egametextures.h"

eDisgruntled::eDisgruntled(eGameBoard& board) :
    eFightingPatroler(board, &eCharacterTextures::fDisgruntled,
                      eCharacterType::disgruntled) {
    eGameTextures::loadDisgruntled();
    setAttack(4);
    setHP(10);
}
