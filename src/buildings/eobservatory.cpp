#include "eobservatory.h"

#include "characters/astronomer.h"
#include "textures/egametextures.h"

eObservatory::eObservatory(GameBoard& board,
                           const eCityId cid) :
    ePatrolTarget(board,
                  &eBuildingTextures::fObservatory,
                  -8.15, -10.65,
                  &eBuildingTextures::fObservatoryOverlay,
                  [this]() { return e::make_shared<Astronomer>(getBoard()); },
                  eBuildingType::observatory, 5, 5, 18, cid) {
    eGameTextures::loadObservatory();
    setOverlaySpeed(0.25);
}
