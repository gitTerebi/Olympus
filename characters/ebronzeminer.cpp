#include "ebronzeminer.h"

#include "textures/egametextures.h"

eBronzeMiner::eBronzeMiner(GameBoard& board) :
    eResourceCollector(board, &eCharacterTextures::fBronzeMiner,
                       eCharacterType::bronzeMiner) {
    eGameTextures::loadBronzeMiner();
}
