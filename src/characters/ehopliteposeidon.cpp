#include "ehopliteposeidon.h"

#include "textures/egametextures.h"

eHoplitePoseidon::eHoplitePoseidon(GameBoard& board) :
    eHopliteBase(board, &eCharacterTextures::fHoplitePoseidon,
                 eCharacterType::hoplitePoseidon) {
    eGameTextures::loadHoplitePoseidon();
}
