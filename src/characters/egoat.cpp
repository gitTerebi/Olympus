#include "egoat.h"

#include "textures/game-textures.h"
#include "numbers.h"

eGoat::eGoat(GameBoard& board) :
    eDomesticatedAnimal(board, &CharacterTextures::fGoat,
                        eCharacterType::goat, Numbers::sGoatMaxGroom) {
    GameTextures::loadGoat();
}
