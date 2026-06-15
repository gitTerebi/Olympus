#include "eatlanteanarcher.h"

#include "textures/game-textures.h"

eAtlanteanArcher::eAtlanteanArcher(GameBoard& board) :
    eArcherBase(board, &CharacterTextures::fAtlanteanArcher,
                eCharacterType::atlanteanArcher) {
    GameTextures::loadAtlanteanArcher();
}
