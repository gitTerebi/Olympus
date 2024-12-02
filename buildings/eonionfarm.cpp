#include "eonionfarm.h"

eOnionFarm::eOnionFarm(eGameBoard& board,
                       const eCityId cid) :
    eFarmBase(board, eBuildingType::onionsFarm, 3, 3,
              eResourceType::onions, cid) {}
