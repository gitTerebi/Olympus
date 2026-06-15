#include "widgets/game-widget.h"

#include "buildings/eheatgetters.h"
#include "buildings/ebuildingrenderer.h"
#include "engine/game-board.h"
#include "textures/terrain-textures.h"
#include "widgets/etilepainter.h"
#include "widgets/paint/draw/dont-draw-appeal.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace {

struct AvenueHeatCell {
    int x;
    int y;
    int value;
};

const std::array<AvenueHeatCell, 54> kAvenueHeat{{
    {-3, -3, 1}, {-2, -3, 2}, {-1, -3, 2}, {0, -3, 2}, {1, -3, 2}, {2, -3, 2}, {3, -3, 2}, {4, -3, 1},
    {-3, -2, 1}, {-2, -2, 3}, {-1, -2, 4}, {0, -2, 4}, {1, -2, 4}, {2, -2, 4}, {3, -2, 3}, {4, -2, 1},
    {-3, -1, 1}, {-2, -1, 3}, {-1, -1, 5}, {0, -1, 6}, {1, -1, 6}, {2, -1, 5}, {3, -1, 3}, {4, -1, 1},
    {-3, 0, 1}, {-2, 0, 3}, {-1, 0, 5}, {2, 0, 5}, {3, 0, 3}, {4, 0, 1},
    {-3, 1, 1}, {-2, 1, 3}, {-1, 1, 5}, {0, 1, 6}, {1, 1, 6}, {2, 1, 5}, {3, 1, 3}, {4, 1, 1},
    {-3, 2, 1}, {-2, 2, 3}, {-1, 2, 4}, {0, 2, 4}, {1, 2, 4}, {2, 2, 4}, {3, 2, 3}, {4, 2, 1},
    {-3, 3, 1}, {-2, 3, 2}, {-1, 3, 2}, {0, 3, 2}, {1, 3, 2}, {2, 3, 2}, {3, 3, 2}, {4, 3, 1},
}};

const std::array<AvenueHeatCell, 96> kBoulevardHeat{{
    {-4, -4, 1}, {-3, -4, 2}, {-2, -4, 3}, {-1, -4, 3}, {0, -4, 3}, {1, -4, 3}, {2, -4, 3}, {3, -4, 3}, {4, -4, 3}, {5, -4, 2}, {6, -4, 1},
    {-4, -3, 1}, {-3, -3, 2}, {-2, -3, 3}, {-1, -3, 3}, {0, -3, 3}, {1, -3, 3}, {2, -3, 3}, {3, -3, 3}, {4, -3, 3}, {5, -3, 2}, {6, -3, 1},
    {-4, -2, 1}, {-3, -2, 2}, {-2, -2, 5}, {-1, -2, 7}, {0, -2, 9}, {1, -2, 9}, {2, -2, 9}, {3, -2, 7}, {4, -2, 5}, {5, -2, 2}, {6, -2, 1},
    {-4, -1, 1}, {-3, -1, 2}, {-2, -1, 5}, {-1, -1, 7}, {0, -1, 9}, {1, -1, 9}, {2, -1, 9}, {3, -1, 7}, {4, -1, 5}, {5, -1, 2}, {6, -1, 1},
    {-4, 0, 1}, {-3, 0, 2}, {-2, 0, 5}, {-1, 0, 7}, {3, 0, 7}, {4, 0, 5}, {5, 0, 2}, {6, 0, 1},
    {-4, 1, 1}, {-3, 1, 2}, {-2, 1, 5}, {-1, 1, 7}, {0, 1, 9}, {1, 1, 9}, {2, 1, 9}, {3, 1, 7}, {4, 1, 5}, {5, 1, 2}, {6, 1, 1},
    {-4, 2, 1}, {-3, 2, 2}, {-2, 2, 5}, {-1, 2, 7}, {0, 2, 9}, {1, 2, 9}, {2, 2, 9}, {3, 2, 7}, {4, 2, 5}, {5, 2, 2}, {6, 2, 1},
    {-4, 3, 1}, {-3, 3, 2}, {-2, 3, 3}, {-1, 3, 3}, {0, 3, 3}, {1, 3, 3}, {2, 3, 3}, {3, 3, 3}, {4, 3, 3}, {5, 3, 2}, {6, 3, 1},
    {-4, 4, 1}, {-3, 4, 2}, {-2, 4, 3}, {-1, 4, 3}, {0, 4, 3}, {1, 4, 3}, {2, 4, 3}, {3, 4, 3}, {4, 4, 3}, {5, 4, 2}, {6, 4, 1},
}};

eTile* avenueRoad(eTile* const tile) {
    if(!tile) return nullptr;
    const auto tl = tile->topLeft<eTile>();
    if(tl && tl->hasRoad()) return tl;
    const auto br = tile->bottomRight<eTile>();
    if(br && br->hasRoad()) return br;
    const auto bl = tile->bottomLeft<eTile>();
    if(bl && bl->hasRoad()) return bl;
    const auto tr = tile->topRight<eTile>();
    if(tr && tr->hasRoad()) return tr;
    return nullptr;
}

}

void GameWidget::paintAppealBuildPreview(eTilePainter& tp,
                                         const TerrainTextures& trrTexs,
                                         eBuilding* const building,
                                         eBuildingRenderer* const renderer,
                                         const int worldTileX, const int worldTileY) {
    if(!building || !renderer) return;

    const auto pid = mBoard->cityIdToPlayerId(mViewedCityId);
    const auto diff = mBoard->difficulty(pid);
    const auto heat = eHeatGetters::appeal(building->type(), diff);
    if(heat.fRange <= 0 || heat.fValue == 0) return;

    const int tileSpanW = renderer->spanW();
    const int tileSpanH = renderer->spanH();
    const int appealRange = heat.fRange;
    const int step = heat.fStepTiles > 0 ? heat.fStepTiles : 1;
    int minStrength = std::abs(heat.fValue);
    int maxStrength = minStrength;
    const int ringCount = (appealRange - 1) / step + 1;
    for(int ring = 1; ring <= ringCount; ring++) {
        const int appealValue = heat.fValue + (ring - 1) * heat.fStepSize;
        if(appealValue == 0) continue;
        const int strength = std::abs(appealValue);
        minStrength = std::min(minStrength, strength);
        maxStrength = std::max(maxStrength, strength);
    }

    int minX;
    int minY;
    int maxX;
    int maxY;
    GameBoard::sBuildTiles(minX, minY, maxX, maxY, worldTileX, worldTileY, tileSpanW, tileSpanH);

    const auto drawPreviewTile = [&](eTile* const tile, const int appealValue) {
        if(!tile) return;
        if(dontDrawAppeal(tile->terrain())) return;
        if(tile->isElevationTile()) return;

        const int strength = std::clamp(std::abs(appealValue), 1, 30);
        const int spread = std::max(1, maxStrength - minStrength);
        const double ratio = std::clamp(
            static_cast<double>(strength - minStrength) / spread, 0., 1.);
        const double appSign = appealValue > 0 ? 1. : -1.;
        const double appS = appSign * pow(abs(appealValue), 0.75);
        int appId = static_cast<int>(std::round(appS + 2.));
        appId = std::clamp(appId, 0, 9);

        const auto tex = trrTexs.fAppeal.getTexture(appId);
        const int alpha = static_cast<int>(appealValue > 0 ?
            20 + 75 * ratio :
            10 + 35 * ratio);
        if(appealValue > 0) {
            tex->setColorMod(0, 255, 0);
        } else {
            tex->setColorMod(255, 120, 120);
        }
        tex->setAlpha(alpha);

        double drawX;
        double drawY;
        const int tileAltitude = mDrawElevation ? tile->altitude() : 0;
        drawXY(tile->x(), tile->y(), drawX, drawY, 1, 1, tileAltitude);
        tp.drawTexture(drawX, drawY, tex, Alignment::top);
        tex->clearAlphaMod();
        tex->clearColorMod();
    };

    if(building->type() == eBuildingType::avenue) {
        minStrength = 1;
        maxStrength = 6;
        const auto avenueTile = mBoard->tile(minX, minY);
        const auto road = avenueRoad(avenueTile);
        if(!avenueTile || !road) return;
        const int ux = road->x() - avenueTile->x();
        const int uy = road->y() - avenueTile->y();
        const int vx = -uy;
        const int vy = ux;
        const auto opposite = road->tileRel<eTile>(ux, uy);
        const bool boulevard = opposite &&
            opposite->underBuildingType() == eBuildingType::avenue;
        maxStrength = boulevard ? 9 : 6;
        const auto drawCell = [&](const AvenueHeatCell& cell) {
            const int dx = cell.x*ux + cell.y*vx;
            const int dy = cell.x*uy + cell.y*vy;
            drawPreviewTile(avenueTile->tileRel<eTile>(dx, dy), cell.value);
        };
        if(boulevard) {
            for(const auto& cell : kBoulevardHeat) drawCell(cell);
        } else {
            for(const auto& cell : kAvenueHeat) drawCell(cell);
        }
        return;
    }

    for(int x = minX - appealRange; x <= minX + tileSpanW + appealRange; x++) {
        for(int y = minY - appealRange; y <= minY + tileSpanH + appealRange; y++) {
            int dx = 0;
            if(x < minX) dx = minX - x;
            else if(x >= minX + tileSpanW) dx = x - (minX + tileSpanW - 1);

            int dy = 0;
            if(y < minY) dy = minY - y;
            else if(y >= minY + tileSpanH) dy = y - (minY + tileSpanH - 1);

            const int dist = std::max(dx, dy);
            if(dist <= 0 || dist > appealRange) continue;

            const int ring = (dist - 1) / step + 1;
            const int appealValue = heat.fValue + (ring - 1) * heat.fStepSize;
            if(appealValue == 0) continue;

            drawPreviewTile(mBoard->tile(x, y), appealValue);
        }
    }
}
