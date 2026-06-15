#include "efleecevendor.h"

#include "textures/game-textures.h"

eFleeceVendor::eFleeceVendor(GameBoard& board,
                             const eCityId cid) :
    eVendor(board,
            eResourceType::fleece, eProvide::fleece,
            &BuildingTextures::fFleeceVendor,
            -2.00, -2.42, &BuildingTextures::fFleeceVendorOverlay,
            -0.50, -2.42, &BuildingTextures::fFleeceVendorOverlay2,
            eBuildingType::fleeceVendor, 2, 2, 4, cid) {
    GameTextures::loadFleeceVendor();
}
