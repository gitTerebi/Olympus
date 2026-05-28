#ifndef EFOUNTAIN_H
#define EFOUNTAIN_H

#include "epatrolbuilding.h"
#include "enumbers.h"

class eFountain : public ePatrolBuilding {
public:
    eFountain(GameBoard& board, const eCityId cid);

    int spawnCooldown() const override { return eNumbers::sWaterCarrierSpawnCooldown; }
};

#endif // EFOUNTAIN_H
