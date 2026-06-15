#ifndef EHOSPITAL_H
#define EHOSPITAL_H

#include "epatrolbuilding.h"
#include "numbers.h"

class eHospital : public ePatrolBuilding {
public:
    eHospital(GameBoard& board, const eCityId cid);

    int spawnCooldown() const override { return Numbers::sInfirmarySpawnCooldown; }
};

#endif // EHOSPITAL_H
