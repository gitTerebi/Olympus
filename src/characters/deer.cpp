#include "deer.h"

#include "textures/egametextures.h"

Deer::Deer(GameBoard& board) :
    eWildAnimal(board, &eCharacterTextures::fDeer,
                eCharacterType::deer) {
    eGameTextures::loadDeer();
    setCityId(eCityId::neutralFriendly);
}
