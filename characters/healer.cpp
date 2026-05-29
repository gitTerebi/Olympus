#include "healer.h"

#include "textures/egametextures.h"

Healer::Healer(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fHealer,
                   eCharacterType::healer) {
    eGameTextures::loadHealer();
    setProvide(eProvide::hygiene, 100000);
}
