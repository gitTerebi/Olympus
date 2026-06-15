#ifndef DISEMBARK_POINT_H
#define DISEMBARK_POINT_H

#include "banner.h"

class DisembarkPoint : public Banner {
public:
    DisembarkPoint(const int id,
                    eTile* const tile,
                    GameBoard& board);
};

#endif // DISEMBARK_POINT_H
