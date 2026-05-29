#ifndef BRONZE_MINER_H
#define BRONZE_MINER_H

#include "eresourcecollector.h"

class BronzeMiner : public eResourceCollector {
public:
    BronzeMiner(GameBoard& board);
};

#endif // BRONZE_MINER_H
