#include "eboar.h"

#include "textures/egametextures.h"

eBoar::eBoar(eGameBoard& board) :
    eWildAnimal(board, &eCharacterTextures::fBoar,
                eCharacterType::boar) {
    eGameTextures::loadBoar();
    setCityId(eCityId::neutralFriendly);
    setHP(100);
    setAttack(0);
}
