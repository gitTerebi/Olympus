#include "earmory.h"

#include "textures/egametextures.h"
#include "enumbers.h"

eArmory::eArmory(eGameBoard& board,
                 const eCityId cid) :
    eProcessingBuilding(board,
                        &eBuildingTextures::fArmory,
                        -1.75, -3.15,
                        &eBuildingTextures::fArmoryOverlay,
                        eBuildingType::armory, 2, 2, 18,
                        eResourceType::bronze,
                        eResourceType::armor, 2,
                        eNumbers::sArmoryProcessingPeriod,
                        cid) {
    eGameTextures::loadArmory();
}
