#ifndef SPEAR_MISSILE_H
#define SPEAR_MISSILE_H

#include "arrow-spear-base.h"

class SpearMissile : public ArrowSpearBase {
public:
    SpearMissile(GameBoard& board,
                  const std::vector<PathPoint>& path = {});
};

#endif // SPEAR_MISSILE_H
