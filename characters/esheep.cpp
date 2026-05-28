#include "esheep.h"

#include "textures/egametextures.h"
#include "enumbers.h"

eSheep::eSheep(GameBoard& board) :
    eDomesticatedAnimal(board, &eCharacterTextures::fNudeSheep,
                        eCharacterType::sheep, eNumbers::sSheepMaxGroom) {
    eGameTextures::loadSheep();
}

void eSheep::setNakedTexture() {
    setTextures(&eCharacterTextures::fNudeSheep);
}

void eSheep::setFleecedTexture() {
    setTextures(&eCharacterTextures::fFleecedSheep);
}
