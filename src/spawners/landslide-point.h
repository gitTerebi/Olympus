#ifndef LANDSLIDE_POINT_H
#define LANDSLIDE_POINT_H

#include "banner.h"

class LandSlidePoint : public Banner {
public:
    LandSlidePoint(const int id,
                    eTile* const tile,
                    GameBoard& board);
};

#endif // LANDSLIDE_POINT_H
