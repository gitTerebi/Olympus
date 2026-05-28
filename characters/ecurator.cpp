#include "ecurator.h"

#include "textures/egametextures.h"

eCurator::eCurator(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fCurator,
                   eCharacterType::curator) {
    eGameTextures::loadCurator();
    setProvide(eProvide::competitorCurator, 10000);
}
