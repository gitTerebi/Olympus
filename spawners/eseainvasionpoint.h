#ifndef ESEAINVASIONPOINT_H
#define ESEAINVASIONPOINT_H

#include "ebanner.h"

class eSeaInvasionPoint : public eBanner {
public:
    eSeaInvasionPoint(const int id,
                      eTile* const tile,
                      GameBoard& board);
};

#endif // ESEAINVASIONPOINT_H
