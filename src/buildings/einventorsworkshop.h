#ifndef EINVENTORSWORKSHOP_H
#define EINVENTORSWORKSHOP_H

#include "epatrolsourcebuilding.h"

class eInventorsWorkshop : public ePatrolSourceBuilding {
public:
    eInventorsWorkshop(GameBoard& board, const eCityId cid);
};

#endif // EINVENTORSWORKSHOP_H
