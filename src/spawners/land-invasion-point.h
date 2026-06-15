#ifndef LAND_INVASION_POINT_H
#define LAND_INVASION_POINT_H

#include "banner.h"

class LandInvasionPoint : public Banner {
public:
    LandInvasionPoint(const int id,
                       eTile* const tile,
                       GameBoard& board);
};

#endif // LAND_INVASION_POINT_H
