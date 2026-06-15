#ifndef DUST_MISSILE_H
#define DUST_MISSILE_H

#include "missile.h"

class DustMissile : public Missile {
public:
    DustMissile(GameBoard& board,
                 const std::vector<PathPoint>& path = {});

    std::shared_ptr<Texture>
    getTexture(const eTileSize size) const override;
private:
    mutable int mCollId;
};

#endif // DUST_MISSILE_H
