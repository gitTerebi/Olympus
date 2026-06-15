#ifndef ARROW_SPEAR_BASE_H
#define ARROW_SPEAR_BASE_H

#include "missile.h"

#include "textures/destruction-textures.h"

class ArrowSpearBase : public Missile {
public:
    using eMissileTexs = TextureCollection DestructionTextures::*;
    ArrowSpearBase(GameBoard& board,
                    const eMissileTexs missileTexs,
                    const MissileType type,
                    const std::vector<PathPoint>& path = {});

    std::shared_ptr<Texture> getTexture(const eTileSize size) const;
private:
    const eMissileTexs mTexs;
};

#endif // ARROW_SPEAR_BASE_H
