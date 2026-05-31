#ifndef EFOUNDRY_H
#define EFOUNDRY_H

#include "eresourcecollectbuilding.h"

class eFoundry : public eResourceCollectBuilding {
public:
    eFoundry(GameBoard& board, const eCityId cid);
};

#endif // EFOUNDRY_H
