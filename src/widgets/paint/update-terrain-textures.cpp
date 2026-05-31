#include "widgets/game-widget.h"

#include "textures/etiletotexture.h"
#include "textures/egametextures.h"
#include "engine/game-board.h"

void GameWidget::updateTerrainTextures(eTile *const tile,
                                        const eTerrainTextures &trrTexs,
                                        const eBuildingTextures &builTexs)
{
    tile->setUnderTile(nullptr);
    auto &painter = tile->terrainPainter();

    painter.fColl = nullptr;
    painter.fTex = eTileToTexture::get(tile, trrTexs, builTexs,
                                       mTileSize, mDrawElevation,
                                       painter.fDrawDim,
                                       &painter.fColl,
                                       mBoard->direction());
}

void GameWidget::updateTerrainTextures()
{
    const int tid = static_cast<int>(mTileSize);
    const auto &trrTexs = eGameTextures::terrain().at(tid);
    const auto &builTexs = eGameTextures::buildings().at(tid);

    mBoard->iterateOverAllTiles([&](eTile *const tile)
                                {
        tile->setDrawDim(1);
        tile->setUnderTile(nullptr); });
    mBoard->iterateOverAllTiles([&](eTile *const tile)
                                { updateTerrainTextures(tile, trrTexs, builTexs); });
}
