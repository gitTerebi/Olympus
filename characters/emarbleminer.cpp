#include "emarbleminer.h"

#include "textures/egametextures.h"

eMarbleMiner::eMarbleMiner(GameBoard& board) :
    eResourceCollector(board, &eCharacterTextures::fMarbleMiner,
                       eCharacterType::marbleMiner) {
    eGameTextures::loadMarbleMiner();
}
