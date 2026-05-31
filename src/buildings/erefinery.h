#ifndef EREFINERY_H
#define EREFINERY_H

#include "eresourcecollectbuilding.h"

class eRefinery : public eResourceCollectBuilding {
public:
    eRefinery(GameBoard& board, const eCityId cid);
};

#endif // EREFINERY_H
