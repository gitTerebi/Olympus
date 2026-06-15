#include "curator.h"

#include "textures/game-textures.h"

Curator::Curator(GameBoard& board) :
    eBasicPatroler(board, &CharacterTextures::fCurator,
                   eCharacterType::curator) {
    GameTextures::loadCurator();
    setProvide(eProvide::competitorCurator, 10000);
}
