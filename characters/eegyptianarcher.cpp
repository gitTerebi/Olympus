#include "eegyptianarcher.h"

#include "textures/egametextures.h"

eEgyptianArcher::eEgyptianArcher(GameBoard& board) :
    eArcherBase(board, &eCharacterTextures::fEgyptianArcher,
                eCharacterType::egyptianArcher) {
    eGameTextures::loadEgyptianArcher();
}
