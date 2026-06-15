#include "ecollege.h"

#include "textures/game-textures.h"

eCollege::eCollege(GameBoard& board,
                   const eCityId cid) :
    ePatrolSourceBuilding(board,
                          &BuildingTextures::fCollege,
                          -2.7, -4.38,
                          &BuildingTextures::fCollegeOverlay,
                          {{eCharacterType::philosopher,
                            eBuildingType::podium}},
                          eBuildingType::college, 3, 3, 12, cid) {
    GameTextures::loadCollege();
}
