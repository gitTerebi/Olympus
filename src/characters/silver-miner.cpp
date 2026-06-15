#include "silver-miner.h"

#include "textures/game-textures.h"

SilverMiner::SilverMiner(GameBoard& board) :
    eResourceCollector(board, &CharacterTextures::fSilverMiner,
                       eCharacterType::silverMiner) {
    GameTextures::loadSilverMiner();
}
