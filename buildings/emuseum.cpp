#include "emuseum.h"

#include "characters/curator.h"
#include "engine/game-board.h"

eMuseum::eMuseum(GameBoard& board,
                 const eCityId cid) :
    ePatrolTarget(board,
                  &eBuildingTextures::fMuseum,
                  -11.55, -12.70,
                  &eBuildingTextures::fMuseumOverlay,
                  [this]() { return e::make_shared<Curator>(getBoard()); },
                  eBuildingType::museum, 6, 6, 50, cid) {
    eGameTextures::loadMuseum();
    board.registerMuseum(this);
}

eMuseum::~eMuseum() {
    auto& b = ownerBoard();
    b.unregisterMuseum(cityId());
}
