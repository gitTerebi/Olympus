#include "efountain.h"

#include "characters/water-distributor.h"
#include "textures/game-textures.h"

eFountain::eFountain(GameBoard& board,
                     const eCityId cid) :
    ePatrolBuilding(board,
                    &BuildingTextures::fFountain,
                    -1.15, -2.37,
                    &BuildingTextures::fFountainOverlay,
                    [this]() { return e::make_shared<WaterDistributor>(getBoard()); },
                    eBuildingType::fountain, 2, 2, 4, cid)  {
    GameTextures::loadFountain();
}
