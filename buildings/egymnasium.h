#ifndef EGYMNASIUM_H
#define EGYMNASIUM_H

#include "epatrolsourcebuilding.h"

class eGymnasium : public ePatrolSourceBuilding {
public:
    eGymnasium(eGameBoard& board, const eCityId cid);
};

#endif // EGYMNASIUM_H
