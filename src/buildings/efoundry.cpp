#include "efoundry.h"

#include "characters/bronze-miner.h"
#include "textures/game-textures.h"

eFoundry::eFoundry(GameBoard& board, const eCityId cid) :
    eResourceCollectBuilding(board,
                             &BuildingTextures::fFoundry,
                             -3.80, -3.78,
                             &BuildingTextures::fFoundryOverlay,
                             2, 1.0, -2.0,
                             [this]() { return e::make_shared<BronzeMiner>(getBoard()); },
                             eBuildingType::foundry,
                             eHasResourceObject::sCreate(eHasResourceObjectType::copper),
                             2, 2, 15, eResourceType::bronze, cid) {
    GameTextures::loadFoundry();
    setRawCountCollect(4);
}
