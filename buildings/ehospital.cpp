#include "ehospital.h"

#include "characters/healer.h"
#include "textures/egametextures.h"

eHospital::eHospital(GameBoard& board,
                     const eCityId cid) :
    ePatrolBuilding(board, &eBuildingTextures::fHospital,
                    -1.84, -4.5,
                    &eBuildingTextures::fHospitalOverlay,
                    [this]() { return e::make_shared<Healer>(getBoard()); },
                    eBuildingType::hospital, 4, 4, 11, cid)  {
    eGameTextures::loadHospital();
}
