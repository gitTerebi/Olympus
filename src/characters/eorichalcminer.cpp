#include "eorichalcminer.h"

#include "textures/egametextures.h"

eOrichalcMiner::eOrichalcMiner(GameBoard& board) :
    eResourceCollector(board, &eCharacterTextures::fOrichalcMiner,
                       eCharacterType::orichalcMiner) {
    eGameTextures::loadOrichalcMiner();
}
