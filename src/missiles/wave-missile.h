#ifndef WAVE_MISSILE_H
#define WAVE_MISSILE_H

#include "missile.h"

class WaveMissile : public Missile {
public:
    WaveMissile(GameBoard& board,
                 const std::vector<PathPoint>& path = {});

    std::shared_ptr<Texture>
    getTexture(const eTileSize size) const override;
};

#endif // WAVE_MISSILE_H
