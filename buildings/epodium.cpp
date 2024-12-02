#include "epodium.h"

#include "characters/ephilosopher.h"
#include "textures/egametextures.h"

ePodium::ePodium(eGameBoard& board,
                 const eCityId cid) :
    ePatrolTarget(board,
                  &eBuildingTextures::fPodium,
                  -2.25, -3.78,
                  &eBuildingTextures::fPodiumOverlay,
                  [this]() { return e::make_shared<ePhilosopher>(getBoard()); },
                  eBuildingType::podium, 2, 2, 4, cid) {
    eGameTextures::loadPodium();
}
