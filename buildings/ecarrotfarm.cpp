#include "ecarrotfarm.h"

eCarrotFarm::eCarrotFarm(GameBoard& board,
                         const eCityId cid) :
    eFarmBase(board, eBuildingType::carrotsFarm, 3, 3,
              eResourceType::carrots, cid) {}

