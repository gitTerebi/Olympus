#ifndef EHOSPITAL_H
#define EHOSPITAL_H

#include "epatrolbuilding.h"
#include "enumbers.h"

class eHospital : public ePatrolBuilding {
public:
    eHospital(eGameBoard& board, const eCityId cid);

    int spawnCooldown() const override { return eNumbers::sInfirmarySpawnCooldown; }
};

#endif // EHOSPITAL_H
