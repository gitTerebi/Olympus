#include "egoat.h"

#include "textures/egametextures.h"
#include "enumbers.h"

eGoat::eGoat(GameBoard& board) :
    eDomesticatedAnimal(board, &eCharacterTextures::fGoat,
                        eCharacterType::goat, eNumbers::sGoatMaxGroom) {
    eGameTextures::loadGoat();
}
