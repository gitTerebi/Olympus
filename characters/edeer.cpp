#include "edeer.h"

#include "textures/egametextures.h"

eDeer::eDeer(GameBoard& board) :
    eWildAnimal(board, &eCharacterTextures::fDeer,
                eCharacterType::deer) {
    eGameTextures::loadDeer();
    setCityId(eCityId::neutralFriendly);
}
