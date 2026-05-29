#include "water-distributor.h"

#include "textures/egametextures.h"

WaterDistributor::WaterDistributor(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fWaterDistributor,
                   eCharacterType::waterDistributor) {
    eGameTextures::loadWaterDistributor();
    setProvide(eProvide::water, 10000);
}
