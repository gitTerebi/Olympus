#include "earmory.h"

#include "textures/game-textures.h"
#include "enumbers.h"

eArmory::eArmory(GameBoard& board,
                 const eCityId cid) :
    eProcessingBuilding(board,
                        &BuildingTextures::fArmory,
                        -1.75, -3.15,
                        &BuildingTextures::fArmoryOverlay,
                        eBuildingType::armory, 2, 2, 18,
                        eResourceType::bronze,
                        eResourceType::armor, 2,
                        eNumbers::sArmoryProcessingPeriod,
                        cid) {
    GameTextures::loadArmory();
}
