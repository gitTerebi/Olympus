#include "ehospital.h"

#include "characters/healer.h"
#include "textures/game-textures.h"

eHospital::eHospital(GameBoard& board,
                     const eCityId cid) :
    ePatrolBuilding(board, &BuildingTextures::fHospital,
                    -1.84, -4.5,
                    &BuildingTextures::fHospitalOverlay,
                    [this]() { return e::make_shared<Healer>(getBoard()); },
                    eBuildingType::hospital, 4, 4, 11, cid)  {
    GameTextures::loadHospital();
}
