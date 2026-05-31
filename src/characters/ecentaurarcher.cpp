#include "ecentaurarcher.h"

#include "textures/egametextures.h"

eCentaurArcher::eCentaurArcher(GameBoard& board) :
    eArcherBase(board, &eCharacterTextures::fCentaurArcher,
                eCharacterType::centaurArcher) {
    eGameTextures::loadCentaurArcher();
    setSpeed(78.75);
}
