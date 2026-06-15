#ifndef LAVA_MISSILE_H
#define LAVA_MISSILE_H

#include "missile.h"

class LavaMissile : public Missile {
public:
    LavaMissile(GameBoard& board,
                 const std::vector<PathPoint>& path = {});

    std::shared_ptr<Texture>
    getTexture(const eTileSize size) const override;
private:
    mutable int mCollId;
    mutable int mTexTimeShift = 0;
};

#endif // LAVA_MISSILE_H
