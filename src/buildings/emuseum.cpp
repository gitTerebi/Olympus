#include "emuseum.h"

#include "characters/curator.h"
#include "engine/game-board.h"

eMuseum::eMuseum(GameBoard& board,
                 const eCityId cid) :
    ePatrolTarget(board,
                  &BuildingTextures::fMuseum,
                  -11.55, -12.70,
                  &BuildingTextures::fMuseumOverlay,
                  [this]() { return e::make_shared<Curator>(getBoard()); },
                  eBuildingType::museum, 6, 6, 50, cid) {
    GameTextures::loadMuseum();
    board.registerMuseum(this);
}

eMuseum::~eMuseum() {
    auto& b = ownerBoard();
    b.unregisterMuseum(cityId());
}
