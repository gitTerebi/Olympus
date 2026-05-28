#include "eox.h"

#include "textures/egametextures.h"

eOx::eOx(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fOx,
                   eCharacterType::ox) {
    eGameTextures::loadOx();
}
