#include "egoat.h"

#include "textures/game-textures.h"
#include "enumbers.h"

eGoat::eGoat(GameBoard& board) :
    eDomesticatedAnimal(board, &CharacterTextures::fGoat,
                        eCharacterType::goat, eNumbers::sGoatMaxGroom) {
    GameTextures::loadGoat();
}
