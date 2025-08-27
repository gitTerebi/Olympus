#ifndef EEXITPOINT_H
#define EEXITPOINT_H

#include "ebanner.h"

class eExitPoint : public eBanner {
public:
    eExitPoint(const int id,
               eTile* const tile,
               eGameBoard& board);
};

class eRiverExitPoint : public eBanner {
public:
    eRiverExitPoint(const int id,
                    eTile* const tile,
                    eGameBoard& board);
};

#endif // EEXITPOINT_H
