#ifndef SEA_INVASION_POINT_H
#define SEA_INVASION_POINT_H

#include "banner.h"

class SeaInvasionPoint : public Banner {
public:
    SeaInvasionPoint(const int id,
                      eTile* const tile,
                      GameBoard& board);
};

#endif // SEA_INVASION_POINT_H
