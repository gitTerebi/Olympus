#ifndef ECOLLEGE_H
#define ECOLLEGE_H

#include "epatrolsourcebuilding.h"

class eCollege : public ePatrolSourceBuilding {
public:
    eCollege(eGameBoard& board, const eCityId cid);
};

#endif // ECOLLEGE_H
