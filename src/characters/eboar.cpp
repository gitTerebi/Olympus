#include "eboar.h"

#include "textures/egametextures.h"

eBoar::eBoar(GameBoard& board) :
    eWildAnimal(board, &eCharacterTextures::fBoar,
    eCharacterType::boar) {
    eGameTextures::loadBoar();
    setCityId(eCityId::neutralFriendly);
}
