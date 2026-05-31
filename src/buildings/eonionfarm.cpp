#include "eonionfarm.h"

eOnionFarm::eOnionFarm(GameBoard& board,
                       const eCityId cid) :
    eFarmBase(board, eBuildingType::onionsFarm, 3, 3,
              eResourceType::onions, cid) {}
