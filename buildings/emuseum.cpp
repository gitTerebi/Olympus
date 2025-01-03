#include "emuseum.h"

#include "characters/ecurator.h"
#include "engine/egameboard.h"

eMuseum::eMuseum(eGameBoard& board,
                 const eCityId cid) :
    ePatrolTarget(board,
                  &eBuildingTextures::fMuseum,
                  -11.55, -12.70,
                  &eBuildingTextures::fMuseumOverlay,
                  [this]() { return e::make_shared<eCurator>(getBoard()); },
                  eBuildingType::museum, 6, 6, 50, cid) {
    eGameTextures::loadMuseum();
    board.registerMuseum(this);
}

eMuseum::~eMuseum() {
    auto& b = getBoard();
    b.unregisterMuseum(cityId());
}
