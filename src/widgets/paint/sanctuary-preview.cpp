#include "widgets/paint/sanctuary-preview.h"

#include "engine/etile.h"
#include "engine/game-board.h"
#include "etilehelper.h"
#include "textures/eterraintextures.h"
#include "widgets/etilepainter.h"

#include <algorithm>

namespace {

struct PreviewTile
{
    int fX;
    int fY;
};

void sortByDrawOrder(std::vector<PreviewTile>& tiles,
                     const eWorldDirection dir,
                     const int boardw,
                     const int boardh)
{
    std::stable_sort(tiles.begin(), tiles.end(),
                     [&](const PreviewTile& lhs, const PreviewTile& rhs)
                     {
                         int ax;
                         int ay;
                         int bx;
                         int by;
                         eTileHelper::dTileIdToRotatedDTileId(
                             lhs.fX, lhs.fY, ax, ay,
                             dir, boardw, boardh);
                         eTileHelper::dTileIdToRotatedDTileId(
                             rhs.fX, rhs.fY, bx, by,
                             dir, boardw, boardh);
                         if (ay != by)
                             return ay < by;
                         return ax < bx;
                     });
}

void previewDrawXY(GameBoard& board,
                   int tx,
                   int ty,
                   double& rx,
                   double& ry,
                   const int wSpan,
                   const int hSpan)
{
    const auto dir = board.direction();
    if (dir != eWorldDirection::N)
    {
        eTileHelper::tileIdToRotatedTileId(tx, ty, tx, ty,
                                           dir, board.width(), board.height());
    }

    rx = tx + 0.5;
    ry = ty + 1.5;

    if (wSpan == 2 && hSpan == 2)       { rx += 0.5; ry += 0.5; }
    else if (wSpan == 3 && hSpan == 3)  { rx += 0.0; ry += 2.0; }
    else if (wSpan == 4 && hSpan == 4)  { rx += 0.5; ry += 2.5; }
    else if (wSpan == 5 && hSpan == 5)  { rx += 0.0; ry += 4.0; }
    else if (wSpan == 6 && hSpan == 6)  { rx += 0.5; ry += 5.5; }
}

}

void drawSanctuaryTerrainPreview(
    GameBoard& board,
    eTilePainter& tp,
    const eTerrainTextures& trrTexs,
    const SDL_Rect footprint,
    const eWorldDirection dir,
    const int boardw,
    const int boardh,
    const bool canBuild)
{
    if (footprint.w <= 0 || footprint.h <= 0)
        return;

    const auto& baseTexture = trrTexs.fBuildingBase;
    std::vector<PreviewTile> tiles;
    tiles.reserve(footprint.w * footprint.h);

    for (int dx = 0; dx < footprint.w; dx++)
    {
        for (int dy = 0; dy < footprint.h; dy++)
        {
            const int x = footprint.x + dx;
            const int y = footprint.y + dy;
            if (!board.tile(x, y))
                continue;
            tiles.push_back({x, y});
        }
    }

    sortByDrawOrder(tiles, dir, boardw, boardh);
    if (canBuild)
        baseTexture->setColorMod(0, 255, 0);
    else
        baseTexture->setColorMod(255, 0, 0);
    for (const auto& tile : tiles)
    {
        double rx;
        double ry;
        previewDrawXY(board, tile.fX, tile.fY, rx, ry, 1, 1);
        tp.drawTexture(rx, ry, baseTexture, eAlignment::top);
    }
    baseTexture->clearColorMod();
}
