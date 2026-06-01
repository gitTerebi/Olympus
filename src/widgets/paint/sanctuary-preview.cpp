#include "widgets/paint/sanctuary-preview.h"

#include "buildings/ebuilding.h"
#include "buildings/ebuildingrenderer.h"
#include "engine/etile.h"
#include "engine/etileterrainpainter.h"
#include "engine/game-board.h"
#include "etilehelper.h"
#include "textures/ebuildingtextures.h"
#include "textures/eterraintextures.h"
#include "textures/etiletotexture.h"
#include "widgets/etilepainter.h"

#include <algorithm>

namespace {

struct TileState
{
    eTile* fTile;
    stdsptr<eBuilding> fUnderBuilding;
    int fSeed;
    int fAltitude;
    bool fWalkableElev;
    bool fUpdateTerrain;
    eTileTerrainPainter fPainter;
};

using TileStates = std::vector<TileState>;

void saveTile(eTile* const tile, TileStates& states)
{
    if (!tile)
        return;
    for (const auto& state : states)
    {
        if (state.fTile == tile)
            return;
    }
    const auto ub = tile->underBuilding();
    states.push_back({tile,
                      ub ? ub->ref<eBuilding>() : nullptr,
                      tile->seed(),
                      tile->altitude(),
                      tile->walkableElev(),
                      tile->updateTerrain(),
                      tile->terrainPainter()});
}

void restoreTiles(const TileStates& states)
{
    for (const auto& state : states)
    {
        state.fTile->terrainPainter() = state.fPainter;
        state.fTile->setUnderBuilding(state.fUnderBuilding);
        state.fTile->setSeed(state.fSeed);
        state.fTile->setAltitude(state.fAltitude, false);
        state.fTile->setWalkableElev(state.fWalkableElev);
        if (state.fUpdateTerrain)
            state.fTile->scheduleTerrainUpdate();
        else
            state.fTile->terrainUpdated();
    }
}

void sortByDrawOrder(TileStates& states,
                     const eWorldDirection dir,
                     const int boardw,
                     const int boardh)
{
    std::stable_sort(states.begin(), states.end(),
                     [&](const TileState& a, const TileState& b)
                     {
                         int ax;
                         int ay;
                         int bx;
                         int by;
                         eTileHelper::dTileIdToRotatedDTileId(
                             a.fTile->dx(), a.fTile->dy(), ax, ay,
                             dir, boardw, boardh);
                         eTileHelper::dTileIdToRotatedDTileId(
                             b.fTile->dx(), b.fTile->dy(), bx, by,
                             dir, boardw, boardh);
                         if (ay != by)
                             return ay < by;
                         return ax < bx;
                     });
}

void setPreviewBuilding(GameBoard& board,
                        TileStates& states,
                        const int x,
                        const int y,
                        const int w,
                        const int h,
                        const stdsptr<eBuilding>& b)
{
    for (int dx = 0; dx < w; dx++)
    {
        for (int dy = 0; dy < h; dy++)
        {
            const auto tile = board.tile(x + dx, y + dy);
            if (!tile)
                continue;
            saveTile(tile, states);
            tile->setUnderBuilding(b);
        }
    }
}

void previewDrawXY(GameBoard& board,
                   int tx,
                   int ty,
                   double& rx,
                   double& ry,
                   const int wSpan,
                   const int hSpan,
                   const int a)
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
    rx -= a;
    ry -= a;
}

}

void drawSanctuaryTerrainPreview(
    GameBoard& board,
    eTilePainter& tp,
    const eTerrainTextures& trrTexs,
    const eBuildingTextures& builTexs,
    const stdsptr<eBuilding>& sanctuary,
    const std::vector<SanctuaryPreviewCell>& cells,
    const std::vector<SanctuaryPreviewBuilding>& buildings,
    const SDL_Rect footprint,
    const eWorldDirection dir,
    const int boardw,
    const int boardh,
    const bool drawElevation,
    const bool canBuild)
{
    if (footprint.w <= 0 || footprint.h <= 0 || !sanctuary)
        return;

    TileStates states;
    states.reserve(footprint.w * footprint.h);

    for (const auto& cell : cells)
    {
        const auto tile = board.tile(cell.fX, cell.fY);
        if (!tile)
            continue;
        saveTile(tile, states);
        tile->setUnderBuilding(sanctuary);
        tile->setAltitude(tile->altitude() + cell.fAltitude, false);
        tile->setSeed(cell.fId);
        if (cell.fType == eSanctEleType::stairs)
            tile->setWalkableElev(true);
    }

    for (const auto& building : buildings)
    {
        if (!building.fBuilding)
            continue;
        const int sw = building.fRenderer ?
            building.fRenderer->spanW() : building.fBuilding->spanW();
        const int sh = building.fRenderer ?
            building.fRenderer->spanH() : building.fBuilding->spanH();
        setPreviewBuilding(board, states, building.fX, building.fY,
                           sw, sh, building.fBuilding);
    }

    sortByDrawOrder(states, dir, boardw, boardh);
    for (const auto& state : states)
    {
        int drawDim = 1;
        const auto tex = eTileToTexture::get(
            state.fTile, trrTexs, builTexs, tp.size(),
            drawElevation, drawDim, nullptr, dir);
        if (!tex)
            continue;
        double rx;
        double ry;
        previewDrawXY(board, state.fTile->x(), state.fTile->y(), rx, ry,
                      drawDim, drawDim,
                      drawElevation ? state.fTile->altitude() : 0);
        if (canBuild)
            tex->setColorMod(0, 255, 0);
        else
            tex->setColorMod(255, 0, 0);
        tp.drawTexture(rx, ry, tex, eAlignment::top);
        tex->clearColorMod();
    }

    restoreTiles(states);
}
