#ifndef EMINT_H
#define EMINT_H

#include "eresourcecollectbuilding.h"

class eMint : public eResourceCollectBuilding {
public:
    eMint(eGameBoard& board, const eCityId cid);
};

#endif // EMINT_H
