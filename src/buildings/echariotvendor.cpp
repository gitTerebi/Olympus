#include "echariotvendor.h"

#include "textures/game-textures.h"

eChariotVendor::eChariotVendor(GameBoard& board,
                               const eCityId cid) :
    eVendor(board,
            eResourceType::chariot, eProvide::horses,
            &BuildingTextures::fChariotVendor,
            -2.75, -3.95, &BuildingTextures::fChariotVendorOverlay,
            -0.45, -2.75, &BuildingTextures::fChariotVendorOverlay2,
            eBuildingType::chariotVendor, 2, 2, 4, cid) {
    GameTextures::loadChariotVendor();
    setResMult(1);
    setMaxRes(4);
}
