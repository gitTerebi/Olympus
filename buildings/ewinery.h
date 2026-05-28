#ifndef EWINERY_H
#define EWINERY_H

#include "eprocessingbuilding.h"

class eWinery : public eProcessingBuilding {
public:
    eWinery(GameBoard& board, const eCityId cid);
};

#endif // EWINERY_H
