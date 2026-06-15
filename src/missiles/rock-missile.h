#ifndef ROCK_MISSILE_H
#define ROCK_MISSILE_H

#include "missile.h"

class RockMissile : public Missile {
public:
    RockMissile(GameBoard& board,
                 const std::vector<PathPoint>& path = {});

    std::shared_ptr<Texture> getTexture(const eTileSize size) const;
};

#endif // ROCK_MISSILE_H
