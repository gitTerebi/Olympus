#include "ewinery.h"

#include "textures/game-textures.h"
#include "numbers.h"

eWinery::eWinery(GameBoard& board, const eCityId cid) :
    eProcessingBuilding(board,
                        &BuildingTextures::fWinery,
                        -2.2, -3.25,
                        &BuildingTextures::fWineryOverlay,
                        eBuildingType::winery, 2, 2, 12,
                        eResourceType::grapes,
                        eResourceType::wine, 1,
                        Numbers::sWineryProcessingPeriod,
                        cid) {
    GameTextures::loadWinery();
}
