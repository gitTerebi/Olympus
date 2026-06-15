#ifndef MARBLE_TILE_H
#define MARBLE_TILE_H

#include "texture.h"
#include "engine/etile.h"
#include "terrain-textures.h"

namespace MarbleTile {
    bool edge(eTile* const tile);
    std::shared_ptr<Texture> get(eTile* const tile,
            const TerrainTextures& textures,
            const eWorldDirection dir);
};

#endif // MARBLE_TILE_H
