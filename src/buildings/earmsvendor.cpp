#include "earmsvendor.h"

#include "textures/game-textures.h"

eArmsVendor::eArmsVendor(GameBoard& board,
                         const eCityId cid) :
    eVendor(board,
            eResourceType::armor, eProvide::arms,
            &BuildingTextures::fArmsVendor,
            -1.86, -2.39, &BuildingTextures::fArmsVendorOverlay,
            0.14, -2.39, &BuildingTextures::fArmsVendorOverlay2,
            eBuildingType::armsVendor, 2, 2, 4, cid) {
    GameTextures::loadArmsVendor();
    setResMult(1);
    setMaxRes(8);
}
