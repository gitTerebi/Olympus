#include "eobservatory.h"

#include "characters/astronomer.h"
#include "textures/game-textures.h"

eObservatory::eObservatory(GameBoard& board,
                           const eCityId cid) :
    ePatrolTarget(board,
                  &BuildingTextures::fObservatory,
                  -8.15, -10.65,
                  &BuildingTextures::fObservatoryOverlay,
                  [this]() { return e::make_shared<Astronomer>(getBoard()); },
                  eBuildingType::observatory, 5, 5, 18, cid) {
    GameTextures::loadObservatory();
    setOverlaySpeed(0.25);
}
