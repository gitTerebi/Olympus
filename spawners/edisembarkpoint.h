#ifndef EDISEMBARKPOINT_H
#define EDISEMBARKPOINT_H

#include "ebanner.h"

class eDisembarkPoint : public eBanner {
public:
    eDisembarkPoint(const int id,
                    eTile* const tile,
                    eGameBoard& board);
};

#endif // EDISEMBARKPOINT_H
