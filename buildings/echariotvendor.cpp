#include "echariotvendor.h"

#include "textures/egametextures.h"

eChariotVendor::eChariotVendor(eGameBoard& board,
                               const eCityId cid) :
    eVendor(board,
            eResourceType::chariot, eProvide::horses,
            &eBuildingTextures::fChariotVendor,
            -2.75, -3.95, &eBuildingTextures::fChariotVendorOverlay,
            -0.45, -2.75, &eBuildingTextures::fChariotVendorOverlay2,
            eBuildingType::chariotVendor, 2, 2, 4, cid) {
    eGameTextures::loadChariotVendor();
    setResMult(1);
    setMaxRes(4);
}
