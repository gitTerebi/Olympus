#include "etheater.h"

#include "characters/actor.h"
#include "textures/egametextures.h"
#include "enumbers.h"

eTheater::eTheater(GameBoard& board, const eCityId cid) :
    ePatrolTarget(board,
                  &eBuildingTextures::fTheater,
                  -1.25, -6.5,
                  &eBuildingTextures::fTheaterOverlay,
                  [this]() { return e::make_shared<Actor>(getBoard()); },
                  eBuildingType::theater, 5, 5, 18, cid) {
    eGameTextures::loadTheater();
}

int eTheater::spawnCooldown() const {
    return eNumbers::sTheaterPatrolSpawnCooldown;
}
