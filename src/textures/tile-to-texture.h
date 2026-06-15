#ifndef TILE_TO_TEXTURE_H
#define TILE_TO_TEXTURE_H

#include "texture.h"

class eTile;
class TerrainTextures;
class BuildingTextures;
class TextureCollection;
enum class eWorldDirection;

enum class eTileSize : int;

namespace TileToTexture {
    std::shared_ptr<Texture> get(eTile* const tile,
                 const TerrainTextures& textures,
                 const BuildingTextures& blds,
                 const eTileSize tileSize,
                 const bool drawElev,
                 int& drawDim,
                 const TextureCollection** coll,
                 const eWorldDirection dir);
};

#endif // TILE_TO_TEXTURE_H
