#include "water-distributor.h"

#include "textures/game-textures.h"

WaterDistributor::WaterDistributor(GameBoard& board) :
    eBasicPatroler(board, &CharacterTextures::fWaterDistributor,
                   eCharacterType::waterDistributor) {
    GameTextures::loadWaterDistributor();
    setProvide(eProvide::water, 10000);
}
