#include "ewatchpost.h"

#include "characters/watchman.h"
#include "textures/egametextures.h"

eWatchpost::eWatchpost(GameBoard& board,
                       const eCityId cid) :
    ePatrolBuilding(board, &eBuildingTextures::fWatchPost,
                    -0.25, -2.1,
                    &eBuildingTextures::fWatchPostOverlay,
                    [this]() { return e::make_shared<Watchman>(getBoard()); },
                    eBuildingType::watchPost, 2, 2, 6, cid)  {
    eGameTextures::loadWatchpost();
}
