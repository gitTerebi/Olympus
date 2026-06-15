#include "emarbleminer.h"

#include "textures/game-textures.h"

eMarbleMiner::eMarbleMiner(GameBoard& board) :
    eResourceCollector(board, &CharacterTextures::fMarbleMiner,
                       eCharacterType::marbleMiner) {
    GameTextures::loadMarbleMiner();
}
