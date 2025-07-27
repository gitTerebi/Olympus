#include "erefinery.h"

#include "characters/eorichalcminer.h"
#include "textures/egametextures.h"

eRefinery::eRefinery(eGameBoard& board, const eCityId cid) :
    eResourceCollectBuilding(board,
                             &eBuildingTextures::fRefinery,
                             -5.47, -5.50,
                             &eBuildingTextures::fRefineryOverlay,
                             2, 1.0, -2.0,
                             [this]() { return e::make_shared<eOrichalcMiner>(getBoard()); },
                             eBuildingType::refinery,
                             eHasResourceObject::sCreate(eHasResourceObjectType::orichalc),
                             2, 2, 16, eResourceType::orichalc, cid) {
    eGameTextures::loadRefinery();
    setRawCountCollect(4);
}
