#include "euniversity.h"

#include "textures/game-textures.h"

eUniversity::eUniversity(GameBoard& board, const eCityId cid) :
    ePatrolSourceBuilding(board,
                          &BuildingTextures::fUniversity,
                          -3.25, -4.00,
                          &BuildingTextures::fUniversityOverlay,
                          {{eCharacterType::astronomer,
                            eBuildingType::observatory},
                           {eCharacterType::curator,
                            eBuildingType::museum}},
                          eBuildingType::university, 3, 3, 12, cid) {
    GameTextures::loadUniversity();
}
