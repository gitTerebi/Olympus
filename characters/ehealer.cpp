#include "ehealer.h"

#include "textures/egametextures.h"

eHealer::eHealer(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fHealer,
                   eCharacterType::healer) {
    eGameTextures::loadHealer();
    setProvide(eProvide::hygiene, 100000);
}
