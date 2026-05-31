#ifndef EUNIVERSITY_H
#define EUNIVERSITY_H

#include "epatrolsourcebuilding.h"

class eUniversity : public ePatrolSourceBuilding {
public:
    eUniversity(GameBoard& board, const eCityId cid);
};

#endif // EUNIVERSITY_H
