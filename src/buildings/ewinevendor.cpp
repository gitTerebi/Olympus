#include "ewinevendor.h"

#include "textures/game-textures.h"

eWineVendor::eWineVendor(GameBoard& board, const eCityId cid) :
    eVendor(board,
            eResourceType::wine, eProvide::wine,
            &BuildingTextures::fWineVendor,
            -2.15, -2.35, &BuildingTextures::fWineVendorOverlay,
            0.35, -2.55, &BuildingTextures::fWineVendorOverlay2,
            eBuildingType::wineVendor, 2, 2, 4, cid) {
    GameTextures::loadWineVendorBuilding();
}
