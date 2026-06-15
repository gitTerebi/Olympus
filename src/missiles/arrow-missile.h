#ifndef ARROW_MISSILE_H
#define ARROW_MISSILE_H

#include "arrow-spear-base.h"

class ArrowMissile : public ArrowSpearBase {
public:
    ArrowMissile(GameBoard& board,
                  const std::vector<PathPoint>& path = {});
};

#endif // ARROW_MISSILE_H
