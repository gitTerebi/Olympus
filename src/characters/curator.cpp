#include "curator.h"

#include "textures/egametextures.h"

Curator::Curator(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fCurator,
                   eCharacterType::curator) {
    eGameTextures::loadCurator();
    setProvide(eProvide::competitorCurator, 10000);
}
