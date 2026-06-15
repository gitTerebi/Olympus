#include "esheep.h"

#include "textures/game-textures.h"
#include "enumbers.h"

eSheep::eSheep(GameBoard& board) :
    eDomesticatedAnimal(board, &CharacterTextures::fNudeSheep,
                        eCharacterType::sheep, eNumbers::sSheepMaxGroom) {
    GameTextures::loadSheep();
}

void eSheep::setNakedTexture() {
    setTextures(&CharacterTextures::fNudeSheep);
}

void eSheep::setFleecedTexture() {
    setTextures(&CharacterTextures::fFleecedSheep);
}
