#include "echariotposeidon.h"

#include "textures/echaractertextures.h"
#include "textures/egametextures.h"

eChariotPoseidon::eChariotPoseidon(GameBoard& board) :
    eChariotBase(board, &eCharacterTextures::fChariotPoseidon,
                 eCharacterType::chariotPoseidon) {
    eGameTextures::loadChariotPoseidon();
}
