#include "emaintenanceoffice.h"

#include "characters/fire-fighter.h"
#include "characters/actions/fire-fighter-action.h"
#include "textures/egametextures.h"

stdsptr<eCharacterAction> gFireFighterActGenerator(
           eCharacter* const c,
           ePatrolBuildingBase* const b,
           const std::vector<eOrientation>& path,
           const stdsptr<eDirectionTimes>& dirTimes) {
    return e::make_shared<FireFighterAction>(c, b, path, dirTimes);
}

eMaintenanceOffice::eMaintenanceOffice(GameBoard& board,
                                       const eCityId cid) :
    ePatrolBuilding(board, &eBuildingTextures::fMaintenanceOffice,
                    -3.85, -4.35,
                    &eBuildingTextures::fMaintenanceOfficeOverlay,
                    [this]() { return e::make_shared<FireFighter>(getBoard()); },
                    gFireFighterActGenerator,
                    eBuildingType::maintenanceOffice, 2, 2, 5, cid)  {
    eGameTextures::loadMaintenanceOffice();
}
