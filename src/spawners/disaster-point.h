#ifndef DISASTER_POINT_H
#define DISASTER_POINT_H

#include "banner.h"

class DisasterPoint : public Banner {
public:
    DisasterPoint(const int id,
                   eTile* const tile,
                   GameBoard& board);
};

#endif // DISASTER_POINT_H
