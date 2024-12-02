#include "ehorsevendor.h"

#include "textures/egametextures.h"

eHorseVendor::eHorseVendor(eGameBoard& board,
                           const eCityId cid) :
    eVendor(board,
            eResourceType::horse, eProvide::horses,
            &eBuildingTextures::fHorseTrainer,
            -1.15, -3.40, &eBuildingTextures::fHorseTrainerOverlay,
            -0.55, -2.5, &eBuildingTextures::fHorseTrainerOverlay2,
            eBuildingType::horseTrainer, 2, 2, 4, cid) {
    eGameTextures::loadHorseVendor();
    setResMult(1);
    setMaxRes(4);
}
