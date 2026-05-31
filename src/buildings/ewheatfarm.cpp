#include "ewheatfarm.h"

eWheatFarm::eWheatFarm(GameBoard& board,
                       const eCityId cid) :
    eFarmBase(board, eBuildingType::wheatFarm, 3, 3,
              eResourceType::wheat, cid) {}
