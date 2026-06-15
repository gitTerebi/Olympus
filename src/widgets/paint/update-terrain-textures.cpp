#include "widgets/game-widget.h"

#include "textures/tile-to-texture.h"
#include "textures/game-textures.h"
#include "engine/game-board.h"

void GameWidget::updateTerrainTextures(eTile *const tile,
                                        const TerrainTextures &trrTexs,
                                        const BuildingTextures &builTexs)
{
    tile->setUnderTile(nullptr);
    auto &painter = tile->terrainPainter();

    painter.fColl = nullptr;
    painter.fTex = TileToTexture::get(tile, trrTexs, builTexs,
                                       mTileSize, mDrawElevation,
                                       painter.fDrawDim,
                                       &painter.fColl,
                                       mBoard->direction());
}

void GameWidget::updateTerrainTextures()
{
    const int tid = static_cast<int>(mTileSize);
    const auto &trrTexs = GameTextures::terrain().at(tid);
    const auto &builTexs = GameTextures::buildings().at(tid);

    mBoard->iterateOverAllTiles([&](eTile *const tile)
                                {
        tile->setDrawDim(1);
        tile->setUnderTile(nullptr); });
    mBoard->iterateOverAllTiles([&](eTile *const tile)
                                { updateTerrainTextures(tile, trrTexs, builTexs); });
}
