#ifndef ELANDINVASIONPOINT_H
#define ELANDINVASIONPOINT_H

#include "ebanner.h"

class eLandInvasionPoint : public eBanner {
public:
    eLandInvasionPoint(const int id,
                       eTile* const tile,
                       GameBoard& board);
};

#endif // ELANDINVASIONPOINT_H
