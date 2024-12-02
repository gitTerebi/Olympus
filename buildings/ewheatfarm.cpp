#include "ewheatfarm.h"

eWheatFarm::eWheatFarm(eGameBoard& board,
                       const eCityId cid) :
    eFarmBase(board, eBuildingType::wheatFarm, 3, 3,
              eResourceType::wheat, cid) {}
