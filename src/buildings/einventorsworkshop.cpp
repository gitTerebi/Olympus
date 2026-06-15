#include "einventorsworkshop.h"

#include "textures/game-textures.h"

eInventorsWorkshop::eInventorsWorkshop(GameBoard& board,
                                       const eCityId cid) :
    ePatrolSourceBuilding(board,
                          &BuildingTextures::fInventorsWorkshop,
                          -1.08, -4.61,
                          &BuildingTextures::fInventorsWorkshopOverlay,
                          {{eCharacterType::inventor,
                            eBuildingType::laboratory}},
                          eBuildingType::inventorsWorkshop, 3, 3, 12, cid) {
    GameTextures::loadInventorsWorkshop();
}
