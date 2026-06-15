#ifndef MONSTER_POINT_H
#define MONSTER_POINT_H

#include "banner.h"

class MonsterPoint : public Banner {
public:
    MonsterPoint(const int id,
                  eTile* const tile,
                  GameBoard& board);
};

#endif // MONSTER_POINT_H
