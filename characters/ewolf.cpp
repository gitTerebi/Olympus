#include "ewolf.h"

#include "textures/egametextures.h"

eWolf::eWolf(eGameBoard& board) :
    eWildAnimal(board, &eCharacterTextures::fWolf,
                eCharacterType::wolf) {
    eGameTextures::loadWolf();
    setCityId(eCityId::neutralAggresive);
    setAttack(0.25);
    setHP(300);
}
