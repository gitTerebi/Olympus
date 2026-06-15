#include "etimbermill.h"

#include "characters/lumberjack.h"
#include "engine/game-board.h"

eTimberMill::eTimberMill(GameBoard& board, const eCityId cid) :
    eResourceCollectBuilding(board,
                             &BuildingTextures::fTimberMill,
                             -3.65, -3.65,
                             &BuildingTextures::fTimberMillOverlay,
                             3, 0.9, -1.1,
                             [this]() { return e::make_shared<Lumberjack>(getBoard()); },
                             eBuildingType::timberMill,
                             eHasResourceObject::sCreate(eHasResourceObjectType::forest),
                             2, 2, 12, eResourceType::wood, cid) {
    GameTextures::loadTimberMill();
    setRawCountCollect(4);
}
