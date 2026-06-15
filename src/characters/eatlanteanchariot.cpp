#include "eatlanteanchariot.h"

#include "textures/character-textures.h"
#include "textures/game-textures.h"

eAtlanteanChariot::eAtlanteanChariot(GameBoard& board) :
    eChariotBase(board, &CharacterTextures::fAtlanteanChariot,
                 eCharacterType::atlanteanChariot) {
    GameTextures::loadAtlanteanChariot();
}
