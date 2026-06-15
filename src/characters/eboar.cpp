#include "eboar.h"

#include "textures/game-textures.h"

eBoar::eBoar(GameBoard& board) :
    eWildAnimal(board, &CharacterTextures::fBoar,
    eCharacterType::boar) {
    GameTextures::loadBoar();
    setCityId(eCityId::neutralFriendly);
}
