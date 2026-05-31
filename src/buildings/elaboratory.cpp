#include "elaboratory.h"

#include "characters/inventor.h"
#include "textures/egametextures.h"

eLaboratory::eLaboratory(GameBoard& board,
                         const eCityId cid) :
    ePatrolTarget(board,
                  &eBuildingTextures::fLaboratory,
                  -5.55, -7.48,
                  &eBuildingTextures::fLaboratoryOverlay,
                  [this]() { return e::make_shared<Inventor>(getBoard()); },
                  eBuildingType::laboratory, 4, 4, 9, cid) {
    eGameTextures::loadLaboratory();
}
