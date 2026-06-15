#include "ehorsevendor.h"

#include "textures/game-textures.h"

eHorseVendor::eHorseVendor(GameBoard& board,
                           const eCityId cid) :
    eVendor(board,
            eResourceType::horse, eProvide::horses,
            &BuildingTextures::fHorseTrainer,
            -1.15, -3.40, &BuildingTextures::fHorseTrainerOverlay,
            -0.55, -2.5, &BuildingTextures::fHorseTrainerOverlay2,
            eBuildingType::horseTrainer, 2, 2, 4, cid) {
    GameTextures::loadHorseVendor();
    setResMult(1);
    setMaxRes(4);
}
