#include "esheep.h"

#include "textures/game-textures.h"
#include "numbers.h"

eSheep::eSheep(GameBoard& board) :
    eDomesticatedAnimal(board, &CharacterTextures::fNudeSheep,
                        eCharacterType::sheep, Numbers::sSheepMaxGroom) {
    GameTextures::loadSheep();
}

void eSheep::setNakedTexture() {
    setTextures(&CharacterTextures::fNudeSheep);
}

void eSheep::setFleecedTexture() {
    setTextures(&CharacterTextures::fFleecedSheep);
}
