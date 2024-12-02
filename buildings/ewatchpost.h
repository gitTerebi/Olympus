#ifndef EWATCHPOST_H
#define EWATCHPOST_H

#include "epatrolbuilding.h"

class eWatchpost : public ePatrolBuilding {
public:
    eWatchpost(eGameBoard& board, const eCityId cid);
};

#endif // EWATCHPOST_H
