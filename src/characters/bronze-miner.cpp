#include "bronze-miner.h"

#include "textures/game-textures.h"

BronzeMiner::BronzeMiner(GameBoard& board) :
    eResourceCollector(board, &CharacterTextures::fBronzeMiner,
                       eCharacterType::bronzeMiner) {
    GameTextures::loadBronzeMiner();
}
