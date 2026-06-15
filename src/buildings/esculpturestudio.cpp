#include "esculpturestudio.h"

#include "textures/game-textures.h"
#include "enumbers.h"

eSculptureStudio::eSculptureStudio(GameBoard& board,
                                   const eCityId cid) :
    eProcessingBuilding(board,
                        &BuildingTextures::fSculptureStudio,
                        -3.73, -4.48,
                        &BuildingTextures::fSculptureStudioOverlay,
                        eBuildingType::sculptureStudio, 2, 2, 12,
                        eResourceType::bronze,
                        eResourceType::sculpture, 4,
                        eNumbers::sSculptureStudioProcessingPeriod,
                        cid) {
    GameTextures::loadSculptureStudio();
}
