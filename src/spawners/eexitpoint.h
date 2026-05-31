#ifndef EEXITPOINT_H
#define EEXITPOINT_H

#include "ebanner.h"

class eExitPoint : public eBanner {
public:
    eExitPoint(const int id,
               eTile* const tile,
               GameBoard& board);
};

class eRiverExitPoint : public eBanner {
public:
    eRiverExitPoint(const int id,
                    eTile* const tile,
                    GameBoard& board);
};

#endif // EEXITPOINT_H
