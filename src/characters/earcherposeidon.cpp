#include "earcherposeidon.h"

#include "textures/game-textures.h"

eArcherPoseidon::eArcherPoseidon(GameBoard& board) :
    eArcherBase(board, &CharacterTextures::fArcherPoseidon,
                eCharacterType::archerPoseidon) {
    GameTextures::loadArcherPoseidon();
}
