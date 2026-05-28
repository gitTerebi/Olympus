#ifndef EMONSTERPOINT_H
#define EMONSTERPOINT_H

#include "ebanner.h"

class eMonsterPoint : public eBanner {
public:
    eMonsterPoint(const int id,
                  eTile* const tile,
                  GameBoard& board);
};

#endif // EMONSTERPOINT_H
