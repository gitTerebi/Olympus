#include "elaboratory.h"

#include "characters/inventor.h"
#include "textures/game-textures.h"

eLaboratory::eLaboratory(GameBoard& board,
                         const eCityId cid) :
    ePatrolTarget(board,
                  &BuildingTextures::fLaboratory,
                  -5.55, -7.48,
                  &BuildingTextures::fLaboratoryOverlay,
                  [this]() { return e::make_shared<Inventor>(getBoard()); },
                  eBuildingType::laboratory, 4, 4, 9, cid) {
    GameTextures::loadLaboratory();
}
