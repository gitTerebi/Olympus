#include "goatherd.h"

#include "textures/game-textures.h"

Goatherd::Goatherd(GameBoard& board) :
    eShepherdBase(board, &CharacterTextures::fGoatherd,
                  eCharacterType::goatherd) {
    GameTextures::loadGoatherd();
}
