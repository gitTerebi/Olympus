#include "deer.h"

#include "textures/game-textures.h"

Deer::Deer(GameBoard& board) :
    eWildAnimal(board, &CharacterTextures::fDeer,
                eCharacterType::deer) {
    GameTextures::loadDeer();
    setCityId(eCityId::neutralFriendly);
}
