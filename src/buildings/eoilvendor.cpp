#include "eoilvendor.h"

#include "textures/game-textures.h"

eOilVendor::eOilVendor(GameBoard& board, const eCityId cid) :
    eVendor(board,
            eResourceType::oliveOil, eProvide::oil,
            &BuildingTextures::fOilVendor,
            -2.56, -3.05, &BuildingTextures::fOilVendorOverlay,
            -0.74, -2.45, &BuildingTextures::fOilVendorOverlay2,
            eBuildingType::oilVendor, 2, 2, 4, cid) {
    GameTextures::loadOilVendor();
}
