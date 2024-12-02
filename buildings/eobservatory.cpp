#include "eobservatory.h"

#include "characters/eastronomer.h"
#include "textures/egametextures.h"

eObservatory::eObservatory(eGameBoard& board,
                           const eCityId cid) :
    ePatrolTarget(board,
                  &eBuildingTextures::fObservatory,
                  -8.15, -10.65,
                  &eBuildingTextures::fObservatoryOverlay,
                  [this]() { return e::make_shared<eAstronomer>(getBoard()); },
                  eBuildingType::observatory, 5, 5, 18, cid) {
    eGameTextures::loadObservatory();
    setOverlaySpeed(0.25);
}
