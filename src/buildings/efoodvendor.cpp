#include "efoodvendor.h"

#include "textures/game-textures.h"

eFoodVendor::eFoodVendor(GameBoard& board,
                         const eCityId cid) :
    eVendor(board,
            eResourceType::food, eProvide::food,
            &BuildingTextures::fFoodVendor,
            -2.41, -2.46, &BuildingTextures::fFoodVendorOverlay,
            0.35, -1.94, &BuildingTextures::fFoodVendorOverlay2,
            eBuildingType::foodVendor, 2, 2, 4, cid) {
    GameTextures::loadFoodVendor();
}
