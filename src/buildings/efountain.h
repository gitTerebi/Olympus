#ifndef EFOUNTAIN_H
#define EFOUNTAIN_H

#include "epatrolbuilding.h"
#include "numbers.h"

class eFountain : public ePatrolBuilding {
public:
    eFountain(GameBoard& board, const eCityId cid);

    int spawnCooldown() const override { return Numbers::sWaterCarrierSpawnCooldown; }
};

#endif // EFOUNTAIN_H
