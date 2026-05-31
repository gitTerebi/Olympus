#ifndef ECOLLEGE_H
#define ECOLLEGE_H

#include "epatrolsourcebuilding.h"

class eCollege : public ePatrolSourceBuilding {
public:
    eCollege(GameBoard& board, const eCityId cid);
};

#endif // ECOLLEGE_H
