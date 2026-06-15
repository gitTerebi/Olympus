#include "epodium.h"

#include "characters/philosopher.h"
#include "textures/game-textures.h"

ePodium::ePodium(GameBoard& board,
                 const eCityId cid) :
    ePatrolTarget(board,
                  &BuildingTextures::fPodium,
                  -2.25, -3.78,
                  &BuildingTextures::fPodiumOverlay,
                  [this]() { return e::make_shared<Philosopher>(getBoard()); },
                  eBuildingType::podium, 2, 2, 4, cid) {
    GameTextures::loadPodium();
}
