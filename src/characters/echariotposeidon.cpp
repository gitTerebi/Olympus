#include "echariotposeidon.h"

#include "textures/character-textures.h"
#include "textures/game-textures.h"

eChariotPoseidon::eChariotPoseidon(GameBoard& board) :
    eChariotBase(board, &CharacterTextures::fChariotPoseidon,
                 eCharacterType::chariotPoseidon) {
    GameTextures::loadChariotPoseidon();
}
