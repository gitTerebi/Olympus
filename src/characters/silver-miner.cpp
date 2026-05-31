#include "silver-miner.h"

#include "textures/egametextures.h"

SilverMiner::SilverMiner(GameBoard& board) :
    eResourceCollector(board, &eCharacterTextures::fSilverMiner,
                       eCharacterType::silverMiner) {
    eGameTextures::loadSilverMiner();
}
