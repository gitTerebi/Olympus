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

void drawXYPreview(int worldTileX,
                   int worldTileY,
                   double& drawX,
                   double& drawY,
                   const int tileSpanW,
                   const int tileSpanH,
                   const int altitude,
                   const eWorldDirection dir,
                   const int boardWidth,
                   const int boardHeight)
{
    int viewTileX = worldTileX;
    int viewTileY = worldTileY;
    if (dir != eWorldDirection::N)
    {
        eTileHelper::tileIdToRotatedTileId(worldTileX, worldTileY,
                                           viewTileX, viewTileY,
                                           dir, boardWidth, boardHeight);
    }

    drawX = viewTileX + 0.5;
    drawY = viewTileY + 1.5;

    if (tileSpanW == 2 && tileSpanH == 2)       { drawX += 0.5; drawY += 0.5; }
    else if (tileSpanW == 3 && tileSpanH == 3)  { drawX += 0.0; drawY += 2.0; }
    else if (tileSpanW == 4 && tileSpanH == 4)  { drawX += 0.5; drawY += 2.5; }
    else if (tileSpanW == 5 && tileSpanH == 5)  { drawX += 0.0; drawY += 4.0; }
    else if (tileSpanW == 6 && tileSpanH == 6)  { drawX += 0.5; drawY += 5.5; }
    drawX -= altitude;
    drawY -= altitude;
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
        const auto& overlayTexture = o.fTex;
        modPreviewTexture(overlayTexture, canBuild);
        if (o.fAlignTop)
            tp.drawTexture(drawX + o.fX, drawY + o.fY, overlayTexture, eAlignment::top);
        else
            tp.drawTexture(drawX + o.fX, drawY + o.fY, overlayTexture);
        overlayTexture->clearColorMod();
    }
}

}

void drawGenericBuildPreviewPart(
    eTilePainter& tp,
    eBuilding* const building,
    eBuildingRenderer* const renderer,
    eTile* const centerTile,
    const int worldTileX,
    const int worldTileY,
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

    const int tileSpanW = renderer->spanW();
    const int tileSpanH = renderer->spanH();
    double drawX;
    double drawY;
    drawXYPreview(worldTileX, worldTileY, drawX, drawY,
                  tileSpanW, tileSpanH, altitude,
                  dir, centerTile->board().width(), centerTile->board().height());
    if (dir == eWorldDirection::E)
    {
        if ((tileSpanW == 4 && tileSpanH == 4) ||
            (tileSpanW == 2 && tileSpanH == 2)) drawX -= 1;
        else if (tileSpanW == 6 && tileSpanH == 6) drawY -= 1;
    }
    else if (dir == eWorldDirection::S)
    {
        if ((tileSpanW == 4 && tileSpanH == 4) ||
            (tileSpanW == 2 && tileSpanH == 2))
        {
            drawX -= 1;
            drawY += 1;
        }
        else if (tileSpanW == 6 && tileSpanH == 6)
        {
            drawX -= 1;
            drawY -= 1;
        }
    }
    else if (dir == eWorldDirection::W)
    {
        if ((tileSpanW == 4 && tileSpanH == 4) ||
            (tileSpanW == 2 && tileSpanH == 2)) drawY += 1;
        else if (tileSpanW == 6 && tileSpanH == 6) drawX -= 1;
    }

    drawPreviewTextureAndOverlays(tp, building, renderer,
                                  drawX, drawY, canBuild, drawOverlays);
}

