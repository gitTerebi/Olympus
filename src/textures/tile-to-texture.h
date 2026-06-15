#ifndef TILE_TO_TEXTURE_H
#define TILE_TO_TEXTURE_H

#include "etexture.h"

class eTile;
class TerrainTextures;
class BuildingTextures;
class eTextureCollection;
enum class eWorldDirection;

enum class eTileSize : int;

namespace TileToTexture {
    std::shared_ptr<eTexture> get(eTile* const tile,
                 const TerrainTextures& textures,
                 const BuildingTextures& blds,
                 const eTileSize tileSize,
                 const bool drawElev,
                 int& drawDim,
                 const eTextureCollection** coll,
                 const eWorldDirection dir);
};

#endif // TILE_TO_TEXTURE_H
