#include "edramaschool.h"

#include "textures/game-textures.h"

eDramaSchool::eDramaSchool(GameBoard& board,
                           const eCityId cid) :
    ePatrolSourceBuilding(board,
                          &BuildingTextures::fDramaSchool,
                          -3.08, -4.51,
                          &BuildingTextures::fDramaSchoolOverlay,
                          {{eCharacterType::actor,
                            eBuildingType::theater}},
                          eBuildingType::dramaSchool, 3, 3, 10, cid) {
    GameTextures::loadDramaSchool();
}
