#include "etheater.h"

#include "characters/actor.h"
#include "textures/game-textures.h"
eTheater::eTheater(GameBoard& board, const eCityId cid) :
    ePatrolTarget(board,
                  &BuildingTextures::fTheater,
                  -1.25, -6.5,
                  &BuildingTextures::fTheaterOverlay,
                  [this]() { return e::make_shared<Actor>(getBoard()); },
                  eBuildingType::theater, 5, 5, 18, cid) {
    GameTextures::loadTheater();
}

