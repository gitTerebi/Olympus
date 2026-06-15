#include "shepherd.h"

#include "textures/game-textures.h"

Shepherd::Shepherd(GameBoard& board) :
    eShepherdBase(board, &CharacterTextures::fShepherd,
                  eCharacterType::shepherd) {
    GameTextures::loadShepherd();
}
