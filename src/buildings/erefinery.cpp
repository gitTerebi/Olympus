#include "erefinery.h"

#include "characters/eorichalcminer.h"
#include "textures/game-textures.h"

eRefinery::eRefinery(GameBoard& board, const eCityId cid) :
    eResourceCollectBuilding(board,
                             &BuildingTextures::fRefinery,
                             -5.47, -5.50,
                             &BuildingTextures::fRefineryOverlay,
                             2, 1.0, -2.0,
                             [this]() { return e::make_shared<eOrichalcMiner>(getBoard()); },
                             eBuildingType::refinery,
                             eHasResourceObject::sCreate(eHasResourceObjectType::orichalc),
                             2, 2, 16, eResourceType::orichalc, cid) {
    GameTextures::loadRefinery();
    setRawCountCollect(4);
}
