#include "ewatchpost.h"

#include "characters/watchman.h"
#include "textures/game-textures.h"

eWatchpost::eWatchpost(GameBoard& board,
                       const eCityId cid) :
    ePatrolBuilding(board, &BuildingTextures::fWatchPost,
                    -0.25, -2.1,
                    &BuildingTextures::fWatchPostOverlay,
                    [this]() { return e::make_shared<Watchman>(getBoard()); },
                    eBuildingType::watchPost, 2, 2, 6, cid)  {
    GameTextures::loadWatchpost();
}
