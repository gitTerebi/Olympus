#include "esilverminer.h"

#include "textures/egametextures.h"

eSilverMiner::eSilverMiner(GameBoard& board) :
    eResourceCollector(board, &eCharacterTextures::fSilverMiner,
                       eCharacterType::silverMiner) {
    eGameTextures::loadSilverMiner();
}
