#include "eorichalcminer.h"

#include "textures/game-textures.h"

eOrichalcMiner::eOrichalcMiner(GameBoard& board) :
    eResourceCollector(board, &CharacterTextures::fOrichalcMiner,
                       eCharacterType::orichalcMiner) {
    GameTextures::loadOrichalcMiner();
}
