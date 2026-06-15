#ifndef EXIT_POINT_H
#define EXIT_POINT_H

#include "banner.h"

class ExitPoint : public Banner {
public:
    ExitPoint(const int id,
               eTile* const tile,
               GameBoard& board);
};

class RiverExitPoint : public Banner {
public:
    RiverExitPoint(const int id,
                    eTile* const tile,
                    GameBoard& board);
};

#endif // EXIT_POINT_H
