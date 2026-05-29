#include "bronze-miner.h"

#include "textures/egametextures.h"

BronzeMiner::BronzeMiner(GameBoard& board) :
    eResourceCollector(board, &eCharacterTextures::fBronzeMiner,
                       eCharacterType::bronzeMiner) {
    eGameTextures::loadBronzeMiner();
}
