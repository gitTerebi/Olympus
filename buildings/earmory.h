#ifndef EARMORY_H
#define EARMORY_H

#include "eprocessingbuilding.h"

class eArmory : public eProcessingBuilding {
public:
    eArmory(eGameBoard& board, const eCityId cid);
};

#endif // EARMORY_H
