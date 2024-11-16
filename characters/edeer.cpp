#include "edeer.h"

#include "textures/egametextures.h"

eDeer::eDeer(eGameBoard& board) :
    eWildAnimal(board, &eCharacterTextures::fDeer,
                eCharacterType::deer) {
    eGameTextures::loadDeer();
    setCityId(eCityId::neutralFriendly);
    setAttack(0);
    setHP(100);
}
