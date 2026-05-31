#include "earcherposeidon.h"

#include "textures/egametextures.h"

eArcherPoseidon::eArcherPoseidon(GameBoard& board) :
    eArcherBase(board, &eCharacterTextures::fArcherPoseidon,
                eCharacterType::archerPoseidon) {
    eGameTextures::loadArcherPoseidon();
}
