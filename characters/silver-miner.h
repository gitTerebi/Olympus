#ifndef SILVER_MINER_H
#define SILVER_MINER_H

#include "eresourcecollector.h"

class SilverMiner : public eResourceCollector {
public:
    SilverMiner(GameBoard& board);
};

#endif // SILVER_MINER_H
