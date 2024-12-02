#ifndef EWINERY_H
#define EWINERY_H

#include "eprocessingbuilding.h"

class eWinery : public eProcessingBuilding {
public:
    eWinery(eGameBoard& board, const eCityId cid);
};

#endif // EWINERY_H
