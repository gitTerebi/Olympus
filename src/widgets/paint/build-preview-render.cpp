#include "widgets/paint/build-preview-render.h"

#include "buildings/ebuilding.h"
#include "buildings/ebuildingrenderer.h"
#include "buildings/epatrolbuilding.h"
#include "engine/etile.h"
#include "engine/game-board.h"
#include "etilehelper.h"
#include "widgets/etilepainter.h"

namespace {

void modPreviewTexture(const std::shared_ptr<eTexture>& tex,
                       const bool canBuild)
{
    if (canBuild) tex->setColorMod(0, 255, 0);
    else          tex->setColorMod(255, 0, 0);
}

void drawXYPreview(int tx,
                   int ty,
                   double& rx,
                   double& ry,
                   const int wSpan,
                   const int hSpan,
                   const int altitude,
                   const eWorldDirection dir,
                   const int boardw,
                   const int boardh)
{
    if (dir != eWorldDirection::N)
    {
        eTileHelper::tileIdToRotatedTileId(tx, ty, tx, ty,
                                           dir, boardw, boardh);
    }

    rx = tx + 0.5;
    ry = ty + 1.5;

    if (wSpan == 2 && hSpan == 2)       { rx += 0.5; ry += 0.5; }
    else if (wSpan == 3 && hSpan == 3)  { rx += 0.0; ry += 2.0; }
    else if (wSpan == 4 && hSpan == 4)  { rx += 0.5; ry += 2.5; }
    else if (wSpan == 5 && hSpan == 5)  { rx += 0.0; ry += 4.0; }
    else if (wSpan == 6 && hSpan == 6)  { rx += 0.5; ry += 5.5; }
    rx -= altitude;
    ry -= altitude;
}

void drawPreviewTextureAndOverlays(eTilePainter& tp,
                                   eBuilding* const building,
                                   eBuildingRenderer* const renderer,
                                   const double drawX,
                                   const double drawY,
                                   const bool canBuild,
                                   const bool drawOverlays)
{
    const auto tex = renderer->getTexture(tp.size());
    if (tex)
    {
        modPreviewTexture(tex, canBuild);
        tp.drawTexture(drawX, drawY, tex, eAlignment::top);
        tex->clearColorMod();
    }

    if (!drawOverlays)
        return;

    const bool skipOverlays = dynamic_cast<ePatrolBuilding*>(building) != nullptr;
    if (skipOverlays)
        return;

    const auto overlays = renderer->getOverlays(tp.size());
    for (const auto& o : overlays)
    {
        const auto& ttex = o.fTex;
        modPreviewTexture(ttex, canBuild);
        if (o.fAlignTop)
            tp.drawTexture(drawX + o.fX, drawY + o.fY, ttex, eAlignment::top);
        else
            tp.drawTexture(drawX + o.fX, drawY + o.fY, ttex);
        ttex->clearColorMod();
    }
}

}

void drawGenericBuildPreviewPart(
    eTilePainter& tp,
    eBuilding* const building,
    eBuildingRenderer* const renderer,
    eTile* const centerTile,
    const int tx,
    const int ty,
    const int altitude,
    const eWorldDirection dir,
    const bool canBuild,
    const bool drawOverlays)
{
    if (!building || !renderer || !centerTile)
        return;

    building->setFrameShift(0);
    building->setSeed(0);
    building->addUnderBuilding(centerTile);
    building->setCenterTile(centerTile);

    const int sw = renderer->spanW();
    const int sh = renderer->spanH();
    double drawX;
    double drawY;
    drawXYPreview(tx, ty, drawX, drawY, sw, sh, altitude,
                  dir, centerTile->board().width(), centerTile->board().height());
    if (dir == eWorldDirection::E)
    {
        if ((sw == 4 && sh == 4) || (sw == 2 && sh == 2)) drawX -= 1;
        else if (sw == 6 && sh == 6) drawY -= 1;
    }
    else if (dir == eWorldDirection::S)
    {
        if ((sw == 4 && sh == 4) || (sw == 2 && sh == 2))
        {
            drawX -= 1;
            drawY += 1;
        }
        else if (sw == 6 && sh == 6)
        {
            drawX -= 1;
            drawY -= 1;
        }
    }
    else if (dir == eWorldDirection::W)
    {
        if ((sw == 4 && sh == 4) || (sw == 2 && sh == 2)) drawY += 1;
        else if (sw == 6 && sh == 6) drawX -= 1;
    }

    drawPreviewTextureAndOverlays(tp, building, renderer,
                                  drawX, drawY, canBuild, drawOverlays);
}

