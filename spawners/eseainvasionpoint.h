#ifndef ESEAINVASIONPOINT_H
#define ESEAINVASIONPOINT_H

#include "ebanner.h"

class eSeaInvasionPoint : public eBanner {
public:
    eSeaInvasionPoint(const int id,
                      eTile* const tile,
                      eGameBoard& board);
};

#endif // ESEAINVASIONPOINT_H
