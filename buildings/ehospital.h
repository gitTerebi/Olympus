#ifndef EHOSPITAL_H
#define EHOSPITAL_H

#include "epatrolbuilding.h"

class eHospital : public ePatrolBuilding {
public:
    eHospital(eGameBoard& board, const eCityId cid);
};

#endif // EHOSPITAL_H
