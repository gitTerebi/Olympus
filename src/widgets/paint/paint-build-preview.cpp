#include "widgets/game-widget.h"

#include "characters/actions/walkable/walkable-object.h"
#include "engine/etile.h"
#include "engine/game-board.h"
#include "numbers.h"
#include "vector-helpers.h"
#include "widgets/etilepainter.h"
#include "widgets/epainter.h"
#include "textures/terrain-textures.h"
#include "textures/building-textures.h"
#include "textures/game-textures.h"
#include "buildings/allbuildings.h"
#include "buildings/ebuildingrenderer.h"
#include "buildings/eagorabase.h"
#include "buildings/eagoraspace.h"
#include "tile-helper.h"
#include "textures/tile-to-texture.h"
#include "widgets/gamebuild/ecommonhousingbuild.h"
#include "widgets/paint/build-preview-render.h"
#include "widgets/paint/sanctuary-preview.h"

#include <algorithm>
#include <set>

void GameWidget::paintBuildPreview(
    eTilePainter& tp,
    ePainter& painter,
    const TerrainTextures& trrTexs,
    const BuildingTextures& builTexs,
    ePlayerId ppid,
    eBuildingMode mode,
    eWorldDirection dir,
    int boardWidth, int boardHeight,
    bool bridgeValid,
    const std::vector<eTile*>& bridgetTs,
    int sMinX, int sMaxX, int sMinY, int sMaxY)
{
    const auto drawBuildText = [&](const std::string &text)
    {
        painter.drawText(mHoverX - mDX + padding(), mHoverY - mDY + padding(), text, FontColor::light);
    };

    const auto drawStampCostEstimate = [&]()
    {
        const auto diff = mBoard->difficulty(ppid);
        painter.drawText(mHoverX - mDX + padding(),
                   mHoverY - mDY + padding(),
                   std::to_string(mStampTool->estimatedCost(diff)),
                   FontColor::red,
                    resolution().fontSizeXS());
    };

    const auto drawBuildDims = [&](const int buildW, const int buildH)
    {
        const auto text = std::to_string(buildW) + " x " + std::to_string(buildH);
        drawBuildText(text);
    };

    const auto drawBuildCount = [&](const int buildCount)
    {
        drawBuildText(std::to_string(buildCount));
    };
    const auto drawStampTemplateSelection = [&]()
    {
        const auto &tex = trrTexs.fBuildingBase;
        tex->setColorMod(0, 96, 255);
        tex->setAlpha(140);
        for(const auto tile : mStampTemplateTiles) {
            if(!tile) continue;
            double drawX;
            double drawY;
            drawXY(tile->x(), tile->y(), drawX, drawY, 1, 1, tile->altitude());
            tp.drawTexture(drawX, drawY, tex, Alignment::top);
        }
        tex->clearAlphaMod();
        tex->clearColorMod();
    };
    if(mCreatingStampTemplate) drawStampTemplateSelection();

    const auto animalBuildTexture = [&](const eBuildingMode mode) {
        auto& charTexs = GameTextures::characters()[static_cast<int>(mTileSize)];
        const AnimalTextures* animalTexs = nullptr;
        const CattleTextures* cattleTexs = nullptr;
        switch(mode) {
        case eBuildingMode::sheep:
            GameTextures::loadSheep();
            animalTexs = &charTexs.fNudeSheep;
            break;
        case eBuildingMode::goat:
            GameTextures::loadGoat();
            animalTexs = &charTexs.fGoat;
            break;
        case eBuildingMode::cattle:
            GameTextures::loadCattle();
            cattleTexs = &charTexs.fCattle2;
            break;
        default:
            break;
        }
        if(animalTexs && !animalTexs->fWalk.empty()) {
            return animalTexs->fWalk[0].getTexture(0);
        }
        if(cattleTexs && !cattleTexs->fWalk.empty()) {
            return cattleTexs->fWalk[0].getTexture(0);
        }
        return std::shared_ptr<Texture>{};
    };
    const auto drawAnimalBuildGhost = [&](eTile* const tile,
                                          const bool valid) {
        if(!tile) return;
        const auto tex = animalBuildTexture(mode);
        if(!tex) return;
        const int worldTileX = tile->x();
        const int worldTileY = tile->y();
        int viewTileX;
        int viewTileY;
        TileHelper::tileIdToRotatedTileId(worldTileX, worldTileY,
                                           viewTileX, viewTileY, dir,
                                           boardWidth, boardHeight);
        const int da = tile->characterDoubleAltitude();
        double x;
        double y;
        if(dir == eWorldDirection::N) {
            x = worldTileX - da*0.5 + 0.75;
            y = worldTileY - da*0.5 + 0.75;
        } else if(dir == eWorldDirection::E) {
            x = viewTileX - da*0.5 + 0.75;
            y = viewTileY - da*0.5 + 0.75;
        } else if(dir == eWorldDirection::S) {
            x = viewTileX - da*0.5 + 0.75;
            y = viewTileY - da*0.5 + 0.75;
        } else {
            x = viewTileX - da*0.5 + 0.75;
            y = viewTileY - da*0.5 + 0.75;
        }
        const double offX = mTileH*tex->offsetX()/30.;
        const double offY = mTileH*tex->offsetY()/30.;
        const int dx = std::round(mDX + 0.5*(x - y)*mTileW - offX);
        const int dy = std::round(mDY + 0.5*(x + y)*mTileH - offY);
        tex->setColorMod(valid ? 0 : 255, valid ? 255 : 0, 0);
        tex->setAlpha(160);
        tex->render(painter.renderer(), dx, dy, false);
        tex->clearAlphaMod();
        tex->clearColorMod();
    };

    const auto t = mBoard->tile(mHoverTX, mHoverTY);
    const int worldTileX = mHoverTX;
    const int worldTileY = mHoverTY;
    const int altitude = t ? t->altitude() : 0;
    const auto drawAgoraRoadAccessPreview = [&](const std::vector<eTile *> &p)
    {
        int ri = 0;
        for (const auto t : p)
        {
            if (!t || !t->hasRoad())
                continue;
            if (ri++ != 3)
                continue;
            drawRoadBands({t}, tp, trrTexs);
            return;
        }
    };

    if ((mode == eBuildingMode::road ||
         mode == eBuildingMode::doricColumn ||
         mode == eBuildingMode::ionicColumn ||
         mode == eBuildingMode::corinthianColumn) &&
        mLeftPressed)
    {
        int buildW = 0;
        int buildH = 0;
        const auto drawBase = [&](eTile *const t)
        {
            const auto &tex = trrTexs.fBuildingBase;
            double drawX;
            double drawY;
            drawXY(t->x(), t->y(), drawX, drawY, 1, 1, t->altitude());
            tp.drawTexture(drawX, drawY, tex, Alignment::top);

            buildW = std::max(buildW, 1 + std::abs(mHoverTX - t->x()));
            buildH = std::max(buildH, 1 + std::abs(mHoverTY - t->y()));
        };
        if (mode == eBuildingMode::road)
        {
            // Preview must walk identical tile set as build path. Skip orient
            // round-trip (lossy on iso row parity) and iterate tiles directly.
            const auto tiles = roadPath();
            if (!tiles.empty())
            {
                const auto &baseTex = trrTexs.fBuildingBase;
                const auto cid = mViewedCityId;
                const auto pid = mBoard->personPlayer();
                for (const auto t : tiles)
                {
                    if (!t) continue;
                    // Tile where a road can't go (building, water, etc): tint
                    // red so the user sees it won't be built. Existing road is
                    // fine. Mirrors the build loop's roadBlocked() test.
                    const bool blocked = roadBlocked(t, cid, pid);
                    if (blocked) baseTex->setColorMod(255, 0, 0);
                    drawBase(t);
                    if (blocked) baseTex->clearColorMod();
                }
                drawBuildDims(buildW, buildH);
                return;
            }
        }
        else
        {
            const auto startTile = mBoard->tile(mHoverTX, mHoverTY);
            std::vector<eOrientation> path;
            const bool r = columnPath(path);
            if (r)
            {
                eTile *t = startTile;
                for (int i = path.size() - 1; i >= 0; i--)
                {
                    if (!t)
                        break;
                    drawBase(t);
                    t = t->neighbour<eTile>(path[i]);
                }
                if (t)
                    drawBase(t);

                drawBuildDims(buildW, buildH);
                return;
            }
        }
    }

    if (mode == eBuildingMode::bridge)
    {
        if (!bridgeValid && bridgetTs.empty())
        {
            GameTextures::loadBridge();
            const auto hoverTile = mBoard->tile(mHoverTX, mHoverTY);
            if (hoverTile)
            {
                const auto &tex = builTexs.fBridge.getTexture(10);
                tex->setColorMod(255, 0, 0);
                double drawX;
                double drawY;
                const int hx = hoverTile->x();
                const int hy = hoverTile->y();
                const int ha = hoverTile->altitude();
                drawXY(hx, hy, drawX, drawY, 1, 1, ha);
                tp.drawTexture(drawX, drawY, tex, Alignment::top);
                tex->clearColorMod();
            }
        }
    }

    if (mLeftPressed)
    {
        if (mode == eBuildingMode::vine ||
            mode == eBuildingMode::oliveTree ||
            mode == eBuildingMode::orangeTree)
        {
            int buildCount = 0;
            std::shared_ptr<Texture> tex;
            if (mode == eBuildingMode::vine)
            {
                tex = builTexs.fVine.getTexture(0);
                tex->setColorMod(0, 255, 0);
            }
            else if (mode == eBuildingMode::oliveTree)
            {
                tex = builTexs.fOliveTree.getTexture(0);
                tex->setColorMod(0, 255, 0);
            }
            else if (mode == eBuildingMode::orangeTree)
            {
                tex = builTexs.fOrangeTree.getTexture(0);
                tex->setColorMod(0, 255, 0);
            }
            else
            {
                tex = trrTexs.fBuildingBase;
            }
            for (int x = sMinX; x <= sMaxX; x++)
            {
                for (int y = sMinY; y <= sMaxY; y++)
                {
                    double drawX;
                    double drawY;
                    const auto t = mBoard->tile(x, y);
                    if (!t)
                        continue;
                    if (t->terrain() != eTerrain::fertile)
                        continue;
                    if (t->underBuilding())
                        continue;
                    const int altitude = t->altitude();
                    drawXY(x, y, drawX, drawY, 1, 1, altitude);
                    tp.drawTexture(drawX, drawY, tex, Alignment::top);

                    buildCount++;
                }
            }
            tex->clearColorMod();
            drawBuildCount(buildCount);
            return;
        }

        if (mode == eBuildingMode::sheep ||
            mode == eBuildingMode::goat ||
            mode == eBuildingMode::cattle)
        {
            int buildCount = 0;
            const auto buildingType = eBuildingModeHelpers::toBuildingType(mode);
            const int allowed = mBoard->countAllowed(mViewedCityId, buildingType);
            const int animalW = 1;
            const int animalH = 1;
            int n = 1;
            for (int x = sMinX; x <= sMaxX; x++)
            {
                for (int y = sMinY; y <= sMaxY; y++)
                {
                    const auto t = mBoard->tile(x, y);
                    if (!t)
                        continue;
                    if (!mBoard->canBuild(x, y, animalW, animalH,
                                          mEditorMode, mViewedCityId, ppid,
                                          true, true))
                        continue;
                    const bool exccess = n > allowed;
                    drawAnimalBuildGhost(t, !exccess);
                    n++;

                    buildCount++;
                }
            }

            drawBuildCount(buildCount);
        }

        if (mode == eBuildingMode::park ||
            mode == eBuildingMode::avenue ||
            mode == eBuildingMode::wall)
        {
            int buildW = 0;
            int buildH = 0;

            const auto &tex = trrTexs.fBuildingBase;
            for (int x = sMinX; x <= sMaxX; x++)
            {
                for (int y = sMinY; y <= sMaxY; y++)
                {
                    double drawX;
                    double drawY;
                    const auto t = mBoard->tile(x, y);
                    if (!t)
                        continue;
                    const auto terr = t->terrain();
                    const bool b = static_cast<bool>(terr & eTerrain::buildable);
                    if (!b)
                        continue;
                    if (t->underBuilding())
                        continue;
                    if (mode == eBuildingMode::avenue)
                    {
                        const bool hr = canBuildAvenue(t, mViewedCityId, ppid,
                                                       mEditorMode);
                        if (!hr)
                            continue;
                    }
                    const int altitude = t->altitude();
                    drawXY(x, y, drawX, drawY, 1, 1, altitude);
                    tp.drawTexture(drawX, drawY, tex, Alignment::top);

                    buildW = 1 + std::max(buildW, std::abs(mHoverTX - t->x()));
                    buildH = 1 + std::max(buildH, std::abs(mHoverTY - t->y()));
                }
            }
            drawBuildDims(buildW, buildH);
            return;
        }

        if (mode == eBuildingMode::commonHousing)
        {
            const auto &tex = trrTexs.fBuildingBase;
            const auto rects = commonHousingBuildRects(
                        mBoard, mViewedCityId, ppid, mEditorMode,
                        mPressedTX, mPressedTY, mHoverTX, mHoverTY);
            for (const auto &rect : rects)
            {
                double drawX;
                double drawY;
                const auto t = mBoard->tile(rect.fX, rect.fY);
                if (!t)
                    continue;
                const int altitude = t->altitude();
                drawXY(rect.fX, rect.fY, drawX, drawY, 1, 1, altitude);
                tp.drawTexture(drawX, drawY, tex, Alignment::top);
                tp.drawTexture(drawX + 1, drawY, tex, Alignment::top);
                tp.drawTexture(drawX, drawY + 1, tex, Alignment::top);
                tp.drawTexture(drawX + 1, drawY + 1, tex, Alignment::top);
            }

            const auto bounds = commonHousingBuildBounds(rects);
            drawBuildDims(bounds.fW / 2, bounds.fH / 2);
            return;
        }
    }

    if (mode == eBuildingMode::sheep ||
        mode == eBuildingMode::goat ||
        mode == eBuildingMode::cattle)
    {
        if (mLeftPressed)
            return;
        const auto buildingType = eBuildingModeHelpers::toBuildingType(mode);
        const int allowed = mBoard->countAllowed(mViewedCityId, buildingType);
        const auto t = mBoard->tile(mHoverTX, mHoverTY);
        if (!t)
            return;
        const int worldTileX = t->x();
        const int worldTileY = t->y();
        const int animalW = 1;
        const int animalH = 1;
        const bool cb = allowed > 0 && mBoard->canBuild(
                                           worldTileX, worldTileY, animalW, animalH,
                                           mEditorMode,
                                           mViewedCityId, ppid,
                                           true, true);
        drawAnimalBuildGhost(t, cb);
        return;
    }

    if (mode == eBuildingMode::stamp)
    {
        paintStampPreview(tp, trrTexs, builTexs, worldTileX, worldTileY, ppid);
        drawStampCostEstimate();
        return;
    }

    switch (mode)
    {
    case eBuildingMode::commonAgora:
    {
        GameTextures::loadAgora();
        const auto &agr = builTexs.fAgora;
        const auto &agrr = builTexs.fAgoraRoad;

        eAgoraOrientation agoraOrientation;
        const auto p = agoraBuildPlaceIter(t, false, agoraOrientation, mViewedCityId, ppid);
        if (p.empty())
        {
            const auto &tex = trrTexs.fBuildingBase;
            tex->setColorMod(255, 0, 0);
            for (int i = worldTileX - 1; i < worldTileX + 2; i++)
            {
                for (int j = worldTileY - 3; j < worldTileY + 3; j++)
                {
                    double drawX;
                    double drawY;
                    drawXY(i, j, drawX, drawY, 1, 1, altitude);
                    tp.drawTexture(drawX, drawY, tex, Alignment::top);
                }
            }
            tex->clearColorMod();
            const int texId = (dir == eWorldDirection::N ||
                               dir == eWorldDirection::S)
                                  ? 0
                                  : 1;
            const auto &road = trrTexs.fRoad.getTexture(texId);
            road->setColorMod(255, 0, 0);
            for (int j = worldTileY - 3; j < worldTileY + 3; j++)
            {
                double drawX;
                double drawY;
                drawXY(worldTileX - 1, j, drawX, drawY, 1, 1, altitude);
                tp.drawTexture(drawX, drawY, road, Alignment::top);
            }
            road->clearColorMod();
        }
        else
        {
            drawAgoraRoadAccessPreview(p);
            if (agoraOrientation == eAgoraOrientation::bottomRight)
            {
                const int iMax = p.size();
                for (int i = 0; i < iMax; i++)
                {
                    const auto t = p[i];
                    stdsptr<Texture> tex;
                    int dim;
                    if (i < 6)
                    {
                        const int texId = t->seed() % agrr.size();
                        tex = agrr.getTexture(texId);
                        dim = 1;
                        if (i == 5)
                            i++;
                    }
                    else if (i < 12)
                    {
                        const int texId = t->seed() % agr.size();
                        tex = agr.getTexture(texId);
                        dim = 2;
                        i++;
                    }
                    else
                    {
                        continue;
                    }
                    double drawX;
                    double drawY;
                    const int worldTileX = t->x();
                    const int worldTileY = t->y();
                    const int altitude = t->altitude();
                    drawXY(worldTileX, worldTileY, drawX, drawY, dim, dim, altitude);
                    if (dim == 2)
                    {
                        if (dir == eWorldDirection::E)
                        {
                            drawX -= 1;
                        }
                        else if (dir == eWorldDirection::S)
                        {
                            drawX -= 1;
                            drawY += 1;
                        }
                        else if (dir == eWorldDirection::W)
                        {
                            drawY += 1;
                        }
                    }
                    tex->setColorMod(0, 255, 0);
                    tp.drawTexture(drawX, drawY, tex, Alignment::top);
                    tex->clearColorMod();
                }
            }
            else if (agoraOrientation == eAgoraOrientation::topLeft)
            {
                const int iMax = p.size();
                for (int i = 0; i < iMax; i++)
                {
                    const auto t = p[i];
                    stdsptr<Texture> tex;
                    int dim;
                    if (i < 6)
                    {
                        const int texId = t->seed() % agrr.size();
                        tex = agrr.getTexture(texId);
                        dim = 1;
                    }
                    else if (i > 12)
                    {
                        const int texId = t->seed() % agr.size();
                        tex = agr.getTexture(texId);
                        dim = 2;
                        i++;
                    }
                    else
                    {
                        continue;
                    }
                    double drawX;
                    double drawY;
                    const int worldTileX = t->x();
                    const int worldTileY = t->y();
                    const int altitude = t->altitude();
                    drawXY(worldTileX, worldTileY, drawX, drawY, dim, dim, altitude);
                    if (dim == 2)
                    {
                        if (dir == eWorldDirection::E)
                        {
                            drawX -= 1;
                        }
                        else if (dir == eWorldDirection::S)
                        {
                            drawX -= 1;
                            drawY += 1;
                        }
                        else if (dir == eWorldDirection::W)
                        {
                            drawY += 1;
                        }
                    }
                    tex->setColorMod(0, 255, 0);
                    tp.drawTexture(drawX, drawY, tex, Alignment::top);
                    tex->clearColorMod();
                }
            }
            else if (agoraOrientation == eAgoraOrientation::bottomLeft)
            {
                const int iMax = p.size();
                for (int i = 0; i < iMax; i++)
                {
                    const auto t = p[i];
                    stdsptr<Texture> tex;
                    int dim;
                    if (i < 6)
                    {
                        const int texId = t->seed() % agrr.size();
                        tex = agrr.getTexture(texId);
                        dim = 1;
                    }
                    else if (i > 11)
                    {
                        const int texId = t->seed() % agr.size();
                        tex = agr.getTexture(texId);
                        dim = 2;
                        i++;
                    }
                    else
                    {
                        continue;
                    }
                    double drawX;
                    double drawY;
                    const int worldTileX = t->x();
                    const int worldTileY = t->y();
                    const int altitude = t->altitude();
                    drawXY(worldTileX, worldTileY, drawX, drawY, dim, dim, altitude);
                    if (dim == 2)
                    {
                        if (dir == eWorldDirection::E)
                        {
                            drawX -= 1;
                        }
                        else if (dir == eWorldDirection::S)
                        {
                            drawX -= 1;
                            drawY += 1;
                        }
                        else if (dir == eWorldDirection::W)
                        {
                            drawY += 1;
                        }
                    }
                    tex->setColorMod(0, 255, 0);
                    tp.drawTexture(drawX, drawY, tex, Alignment::top);
                    tex->clearColorMod();
                }
            }
            else if (agoraOrientation == eAgoraOrientation::topRight)
            {
                const int iMax = p.size();
                for (int i = 0; i < iMax; i++)
                {
                    const auto t = p[i];
                    stdsptr<Texture> tex;
                    int dim;
                    if (i < 6)
                    {
                        const int texId = t->seed() % agrr.size();
                        tex = agrr.getTexture(texId);
                        dim = 1;
                    }
                    else if (i < 12)
                    {
                        const int texId = t->seed() % agr.size();
                        tex = agr.getTexture(texId);
                        dim = 2;
                        i++;
                    }
                    else
                    {
                        continue;
                    }
                    double drawX;
                    double drawY;
                    const int worldTileX = t->x();
                    const int worldTileY = t->y();
                    const int altitude = t->altitude();
                    drawXY(worldTileX, worldTileY, drawX, drawY, dim, dim, altitude);
                    if (dim == 2)
                    {
                        if (dir == eWorldDirection::E)
                        {
                            drawX -= 1;
                        }
                        else if (dir == eWorldDirection::S)
                        {
                            drawX -= 1;
                            drawY += 1;
                        }
                        else if (dir == eWorldDirection::W)
                        {
                            drawY += 1;
                        }
                    }
                    tex->setColorMod(0, 255, 0);
                    tp.drawTexture(drawX, drawY, tex, Alignment::top);
                    tex->clearColorMod();
                }
            }
        }
    }
    break;
    case eBuildingMode::grandAgora:
    {
        GameTextures::loadAgora();
        const auto &agr = builTexs.fAgora;
        const auto &agrr = builTexs.fAgoraRoad;

        eAgoraOrientation agoraOrientation;
        const auto p = agoraBuildPlaceIter(t, true, agoraOrientation, mViewedCityId, ppid);
        if (p.empty())
        {
            const auto &tex = trrTexs.fBuildingBase;
            tex->setColorMod(255, 0, 0);
            for (int i = worldTileX - 2; i < worldTileX + 3; i++)
            {
                for (int j = worldTileY - 3; j < worldTileY + 3; j++)
                {
                    double drawX;
                    double drawY;
                    drawXY(i, j, drawX, drawY, 1, 1, altitude);
                    tp.drawTexture(drawX, drawY, tex, Alignment::top);
                }
            }
            tex->clearColorMod();
            const int texId = (dir == eWorldDirection::N ||
                               dir == eWorldDirection::S)
                                  ? 0
                                  : 1;
            const auto &road = trrTexs.fRoad.getTexture(texId);
            road->setColorMod(255, 0, 0);
            for (int j = worldTileY - 3; j < worldTileY + 3; j++)
            {
                double drawX;
                double drawY;
                drawXY(worldTileX, j, drawX, drawY, 1, 1, altitude);
                tp.drawTexture(drawX, drawY, road, Alignment::top);
            }
            road->clearColorMod();
        }
        else
        {
            drawAgoraRoadAccessPreview(p);
            if (agoraOrientation == eAgoraOrientation::bottomRight)
            {
                const int iMax = p.size();
                for (int i = 0; i < iMax; i++)
                {
                    const auto t = p[i];
                    const int worldTileX = t->x();
                    const int worldTileY = t->y();
                    const int altitude = t->altitude();
                    stdsptr<Texture> tex;
                    int dim;
                    if (i < 6)
                    {
                        const int texId = t->seed() % agrr.size();
                        tex = agrr.getTexture(texId);
                        dim = 1;
                    }
                    else if ((i > 12 && i < 18) || (i > 24 && i < 30))
                    {
                        const int texId = t->seed() % agr.size();
                        tex = agr.getTexture(texId);
                        dim = 2;
                        i++;
                    }
                    else
                    {
                        continue;
                    }
                    double drawX;
                    double drawY;
                    drawXY(worldTileX, worldTileY, drawX, drawY, dim, dim, altitude);
                    if (dim == 2)
                    {
                        if (dir == eWorldDirection::E)
                        {
                            drawX -= 1;
                        }
                        else if (dir == eWorldDirection::S)
                        {
                            drawX -= 1;
                            drawY += 1;
                        }
                        else if (dir == eWorldDirection::W)
                        {
                            drawY += 1;
                        }
                    }
                    tex->setColorMod(0, 255, 0);
                    tp.drawTexture(drawX, drawY, tex, Alignment::top);
                    tex->clearColorMod();
                }
            }
            else if (agoraOrientation == eAgoraOrientation::bottomLeft)
            {
                const int iMax = p.size();
                for (int i = 0; i < iMax; i++)
                {
                    const auto t = p[i];
                    const int worldTileX = t->x();
                    const int worldTileY = t->y();
                    const int altitude = t->altitude();
                    stdsptr<Texture> tex;
                    int dim;
                    if (i < 6)
                    {
                        const int texId = t->seed() % agrr.size();
                        tex = agrr.getTexture(texId);
                        dim = 1;
                    }
                    else if ((i < 12) || (i > 29 && i < 35))
                    {
                        const int texId = t->seed() % agr.size();
                        tex = agr.getTexture(texId);
                        dim = 2;
                        i++;
                    }
                    else
                    {
                        continue;
                    }
                    double drawX;
                    double drawY;
                    drawXY(worldTileX, worldTileY, drawX, drawY, dim, dim, altitude);
                    if (dim == 2)
                    {
                        if (dir == eWorldDirection::E)
                        {
                            drawX -= 1;
                        }
                        else if (dir == eWorldDirection::S)
                        {
                            drawX -= 1;
                            drawY += 1;
                        }
                        else if (dir == eWorldDirection::W)
                        {
                            drawY += 1;
                        }
                    }
                    tex->setColorMod(0, 255, 0);
                    tp.drawTexture(drawX, drawY, tex, Alignment::top);
                    tex->clearColorMod();
                }
            }
        }
    }
    break;
    default:
        break;
    }

    const auto &wrld = mBoard->world();
    if (t && mGm->visible())
    {
        bool fertile = false;
        std::function<bool(const int worldTileX, const int worldTileY,
                           const int tileSpanW, const int tileSpanH)>
            canBuildFunc;
        switch (mode)
        {
        case eBuildingMode::urchinQuay:
        case eBuildingMode::fishery:
        {
            canBuildFunc = [&](const int worldTileX, const int worldTileY,
                               const int tileSpanW, const int tileSpanH)
            {
                (void)tileSpanW;
                (void)tileSpanH;
                eDiagonalOrientation o;
                return canBuildFishery(worldTileX, worldTileY, o);
            };
        }
        break;
        case eBuildingMode::triremeWharf:
        {
            canBuildFunc = [&](const int worldTileX, const int worldTileY,
                               const int tileSpanW, const int tileSpanH)
            {
                (void)tileSpanW;
                (void)tileSpanH;
                eDiagonalOrientation o;
                return canBuildTriremeWharf(worldTileX, worldTileY, o);
            };
        }
        break;
        case eBuildingMode::pier:
        {
            canBuildFunc = [&](const int worldTileX, const int worldTileY,
                               const int tileSpanW, const int tileSpanH)
            {
                if (tileSpanW > 2 || tileSpanH > 2)
                    return true;
                eDiagonalOrientation o;
                return canBuildPier(worldTileX, worldTileY, o, mViewedCityId, ppid, mEditorMode);
            };
        }
        break;
        case eBuildingMode::palace:
        {
            canBuildFunc = [&](const int worldTileX, const int worldTileY,
                               const int tileSpanW, const int tileSpanH)
            {
                if (mBoard->hasPalace(mViewedCityId))
                    return false;
                return mBoard->canBuild(worldTileX, worldTileY, tileSpanW, tileSpanH,
                                        mEditorMode,
                                        mViewedCityId, ppid,
                                        fertile);
            };
        }
        break;
        case eBuildingMode::stadium:
        {
            canBuildFunc = [&](const int worldTileX, const int worldTileY,
                               const int tileSpanW, const int tileSpanH)
            {
                if (mBoard->hasStadium(mViewedCityId))
                    return false;
                return mBoard->canBuild(worldTileX, worldTileY, tileSpanW, tileSpanH,
                                        mEditorMode,
                                        mViewedCityId, ppid,
                                        fertile);
            };
        }
        break;
        case eBuildingMode::foodVendor:
        {
            canBuildFunc = [&](const int worldTileX, const int worldTileY,
                               const int tileSpanW, const int tileSpanH)
            {
                (void)tileSpanW;
                (void)tileSpanH;
                return canBuildVendor(worldTileX, worldTileY, eResourceType::food);
            };
        }
        break;
        case eBuildingMode::fleeceVendor:
        {
            canBuildFunc = [&](const int worldTileX, const int worldTileY,
                               const int tileSpanW, const int tileSpanH)
            {
                (void)tileSpanW;
                (void)tileSpanH;
                return canBuildVendor(worldTileX, worldTileY, eResourceType::fleece);
            };
        }
        break;
        case eBuildingMode::oilVendor:
        {
            canBuildFunc = [&](const int worldTileX, const int worldTileY,
                               const int tileSpanW, const int tileSpanH)
            {
                (void)tileSpanW;
                (void)tileSpanH;
                return canBuildVendor(worldTileX, worldTileY, eResourceType::oliveOil);
            };
        }
        break;
        case eBuildingMode::wineVendor:
        {
            canBuildFunc = [&](const int worldTileX, const int worldTileY,
                               const int tileSpanW, const int tileSpanH)
            {
                (void)tileSpanW;
                (void)tileSpanH;
                return canBuildVendor(worldTileX, worldTileY, eResourceType::wine);
            };
        }
        break;
        case eBuildingMode::armsVendor:
        {
            canBuildFunc = [&](const int worldTileX, const int worldTileY,
                               const int tileSpanW, const int tileSpanH)
            {
                (void)tileSpanW;
                (void)tileSpanH;
                return canBuildVendor(worldTileX, worldTileY, eResourceType::armor);
            };
        }
        break;
        case eBuildingMode::horseTrainer:
        {
            canBuildFunc = [&](const int worldTileX, const int worldTileY,
                               const int tileSpanW, const int tileSpanH)
            {
                (void)tileSpanW;
                (void)tileSpanH;
                return canBuildVendor(worldTileX, worldTileY, eResourceType::horse);
            };
        }
        break;
        case eBuildingMode::chariotVendor:
        {
            canBuildFunc = [&](const int worldTileX, const int worldTileY,
                               const int tileSpanW, const int tileSpanH)
            {
                (void)tileSpanW;
                (void)tileSpanH;
                return canBuildVendor(worldTileX, worldTileY, eResourceType::chariot);
            };
        }
        break;
        case eBuildingMode::avenue:
        {
            canBuildFunc = [&](const int worldTileX, const int worldTileY,
                               const int tileSpanW, const int tileSpanH)
            {
                (void)tileSpanW;
                (void)tileSpanH;
                const auto t = mBoard->tile(worldTileX, worldTileY);
                return canBuildAvenue(t, mViewedCityId, ppid, mEditorMode);
            };
        }
        break;
        default:
        {
            canBuildFunc = [&](const int worldTileX, const int worldTileY,
                               const int tileSpanW, const int tileSpanH)
            {
                return mBoard->canBuild(worldTileX, worldTileY, tileSpanW, tileSpanH,
                                        mEditorMode,
                                        mViewedCityId, ppid,
                                        fertile);
            };
        }
        break;
        }

        struct eB
        {
            eB(const int worldTileX, const int worldTileY,
               const stdsptr<eBuilding> &b,
               const int altitude = 0,
               const int templeOverlayDirId = -1,
               const GodType statueGod = GodType::zeus,
               const int statueTextureId = -1,
               const int monumentTextureId = -1,
               const bool altar = false,
               const int order = 0) :
                fTx(worldTileX), fTy(worldTileY), fAltitude(altitude),
                fTempleOverlayDirId(templeOverlayDirId),
                fStatueGod(statueGod),
                fStatueTextureId(statueTextureId),
                fMonumentTextureId(monumentTextureId),
                fAltar(altar), fOrder(order), fB(b), fTorch(false) {}

            int fTx;
            int fTy;
            int fAltitude;
            int fTempleOverlayDirId;
            GodType fStatueGod;
            int fStatueTextureId;
            int fMonumentTextureId;
            bool fAltar;
            bool fTorch;
            int fOrder;
            stdsptr<eBuilding> fB;
            stdsptr<eBuildingRenderer> fBR;
        };
        const auto drawRoadAccessPreview = [&](const std::vector<eB> &ebs,
                                               const bool canBuild)
        {
            if (!canBuild)
                return;
            const auto patrolBuilding = [](eBuilding *const b)
            {
                if (const auto pb = dynamic_cast<ePatrolBuildingBase *>(b))
                {
                    return pb;
                }
                else if (const auto v = dynamic_cast<eVendor *>(b))
                {
                    return static_cast<ePatrolBuildingBase *>(v->agora());
                }
                else if (const auto s = dynamic_cast<eAgoraSpace *>(b))
                {
                    return static_cast<ePatrolBuildingBase *>(s->agora());
                }
                return static_cast<ePatrolBuildingBase *>(nullptr);
            };
            std::set<eBuilding *> done;
            for (const auto &eb : ebs)
            {
                const auto pb = patrolBuilding(eb.fB.get());
                if (!pb)
                    continue;
                if (done.count(pb))
                    continue;
                done.insert(pb);
                if (!eb.fBR)
                    continue;
                const int tileSpanW = eb.fBR->spanW();
                const int tileSpanH = eb.fBR->spanH();
                int minX;
                int minY;
                int maxX;
                int maxY;
                GameBoard::sBuildTiles(minX, minY, maxX, maxY,
                                        eb.fTx, eb.fTy, tileSpanW, tileSpanH);
                pb->setTileRect({minX, minY, tileSpanW, tileSpanH});
                eRoadPreviewPath path;
                eTile* spawnTile = nullptr;
                addPatrolBuildingRoadPreview(pb, path, spawnTile);
                drawRoadPreview(path, spawnTile, tp, trrTexs);
            }
        };

        std::vector<eB> ebs;
        SDL_Rect previewFootprint{0, 0, 0, 0};
        switch (mode)
        {
        case eBuildingMode::modestPyramid:
        case eBuildingMode::pyramid:
        case eBuildingMode::greatPyramid:
        case eBuildingMode::majesticPyramid:

        case eBuildingMode::smallMonumentToTheSky:
        case eBuildingMode::monumentToTheSky:
        case eBuildingMode::grandMonumentToTheSky:

        case eBuildingMode::minorShrineAphrodite:
        case eBuildingMode::minorShrineApollo:
        case eBuildingMode::minorShrineAres:
        case eBuildingMode::minorShrineArtemis:
        case eBuildingMode::minorShrineAthena:
        case eBuildingMode::minorShrineAtlas:
        case eBuildingMode::minorShrineDemeter:
        case eBuildingMode::minorShrineDionysus:
        case eBuildingMode::minorShrineHades:
        case eBuildingMode::minorShrineHephaestus:
        case eBuildingMode::minorShrineHera:
        case eBuildingMode::minorShrineHermes:
        case eBuildingMode::minorShrinePoseidon:
        case eBuildingMode::minorShrineZeus:

        case eBuildingMode::shrineAphrodite:
        case eBuildingMode::shrineApollo:
        case eBuildingMode::shrineAres:
        case eBuildingMode::shrineArtemis:
        case eBuildingMode::shrineAthena:
        case eBuildingMode::shrineAtlas:
        case eBuildingMode::shrineDemeter:
        case eBuildingMode::shrineDionysus:
        case eBuildingMode::shrineHades:
        case eBuildingMode::shrineHephaestus:
        case eBuildingMode::shrineHera:
        case eBuildingMode::shrineHermes:
        case eBuildingMode::shrinePoseidon:
        case eBuildingMode::shrineZeus:

        case eBuildingMode::majorShrineAphrodite:
        case eBuildingMode::majorShrineApollo:
        case eBuildingMode::majorShrineAres:
        case eBuildingMode::majorShrineArtemis:
        case eBuildingMode::majorShrineAthena:
        case eBuildingMode::majorShrineAtlas:
        case eBuildingMode::majorShrineDemeter:
        case eBuildingMode::majorShrineDionysus:
        case eBuildingMode::majorShrineHades:
        case eBuildingMode::majorShrineHephaestus:
        case eBuildingMode::majorShrineHera:
        case eBuildingMode::majorShrineHermes:
        case eBuildingMode::majorShrinePoseidon:
        case eBuildingMode::majorShrineZeus:

        case eBuildingMode::pyramidToThePantheon:
        case eBuildingMode::altarOfOlympus:
        case eBuildingMode::templeOfOlympus:
        case eBuildingMode::observatoryKosmika:
        case eBuildingMode::museumAtlantika:
        {
            const auto &tex = trrTexs.fBuildingBase;
            const auto type = eBuildingModeHelpers::toBuildingType(mode);
            int tileSpanW;
            int tileSpanH;
            ePyramid::sDimensions(type, tileSpanW, tileSpanH);
            const int xMin = mHoverTX - tileSpanW / 2;
            const int yMin = mHoverTY - tileSpanH / 2;
            const int xMax = xMin + tileSpanW;
            const int yMax = yMin + tileSpanH;
            const bool cb = mBoard->canBuildBase(xMin, xMax, yMin, yMax,
                                                 mEditorMode,
                                                 mViewedCityId, ppid);
            if (!cb)
                tex->setColorMod(255, 0, 0);
            for (int x = xMin; x < xMax; x++)
            {
                for (int y = yMin; y < yMax; y++)
                {
                    double drawX;
                    double drawY;
                    const auto t = mBoard->tile(x, y);
                    if (!t)
                        continue;
                    if (t->underBuilding())
                        continue;
                    const int altitude = t->altitude();
                    drawXY(x, y, drawX, drawY, 1, 1, altitude);
                    tp.drawTexture(drawX, drawY, tex, Alignment::top);
                }
            }
            if (!cb)
                tex->clearColorMod();
        }
        break;
        case eBuildingMode::templeAphrodite:
        case eBuildingMode::templeApollo:
        case eBuildingMode::templeAres:
        case eBuildingMode::templeArtemis:
        case eBuildingMode::templeAthena:
        case eBuildingMode::templeAtlas:
        case eBuildingMode::templeDemeter:
        case eBuildingMode::templeDionysus:
        case eBuildingMode::templeHades:
        case eBuildingMode::templeHephaestus:
        case eBuildingMode::templeHera:
        case eBuildingMode::templeHermes:
        case eBuildingMode::templePoseidon:
        case eBuildingMode::templeZeus:
        {
            const auto sanctuaryEntries = createSanctuaryPreviewEntries(
                *mBoard, mode, mRotateId, mHoverTX, mHoverTY,
                mViewedCityId, previewFootprint);
            for(const auto& entry : sanctuaryEntries) {
                auto& eb = ebs.emplace_back(entry.fTx, entry.fTy, entry.fB,
                                            entry.fAltitude,
                                            entry.fTempleOverlayDirId,
                                            entry.fStatueGod,
                                            entry.fStatueTextureId,
                                            entry.fMonumentTextureId,
                                            entry.fAltar,
                                            entry.fOrder);
                eb.fTorch = entry.fTorch;
            }
            canBuildFunc = [&, previewFootprint](
                               const int, const int, const int, const int)
            {
                return mBoard->canBuildBase(
                    previewFootprint.x,
                    previewFootprint.x + previewFootprint.w,
                    previewFootprint.y,
                    previewFootprint.y + previewFootprint.h,
                                            mEditorMode, mViewedCityId, ppid);
            };
        }
        break;
        case eBuildingMode::road:
        {
            const auto b1 = e::make_shared<eRoad>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::bridge:
        {
        }
        break;
        case eBuildingMode::roadblock:
        {
            const auto b1 = e::make_shared<eRoad>(*mBoard, mViewedCityId);
            b1->setRoadblock(true);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
            canBuildFunc = [&](const int worldTileX, const int worldTileY,
                               const int tileSpanW, const int tileSpanH)
            {
                (void)tileSpanW;
                (void)tileSpanH;
                const auto t = mBoard->tile(worldTileX, worldTileY);
                if (!t)
                    return false;
                const bool hr = t->hasRoad();
                if (!hr)
                    return false;
                const bool b = t->hasBridge();
                if (b)
                    return false;
                const auto building = t->underBuilding();
                const auto r = static_cast<eRoad *>(building);
                return !r->isRoadblock();
            };
        }
        break;
        case eBuildingMode::achillesHall:
        case eBuildingMode::atalantaHall:
        case eBuildingMode::bellerophonHall:
        case eBuildingMode::herculesHall:
        case eBuildingMode::jasonHall:
        case eBuildingMode::odysseusHall:
        case eBuildingMode::perseusHall:
        case eBuildingMode::theseusHall:
        {
            const auto hallType = eBuildingModeHelpers::toBuildingType(mode);
            const auto heroType = eHerosHall::sHallTypeToHeroType(hallType);
            const auto b1 = e::make_shared<eHerosHall>(heroType, *mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::commonHousing:
        {
            const auto b1 = e::make_shared<SmallHouse>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::gymnasium:
        {
            const auto b1 = e::make_shared<eGymnasium>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::podium:
        {
            const auto b1 = e::make_shared<ePodium>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::fountain:
        {
            const auto b1 = e::make_shared<eFountain>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::watchpost:
        {
            const auto b1 = e::make_shared<eWatchpost>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::maintenanceOffice:
        {
            const auto b1 = e::make_shared<eMaintenanceOffice>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::college:
        {
            const auto b1 = e::make_shared<eCollege>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::dramaSchool:
        {
            const auto b1 = e::make_shared<eDramaSchool>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::theater:
        {
            const auto b1 = e::make_shared<eTheater>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::hospital:
        {
            const auto b1 = e::make_shared<eHospital>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::stadium:
        {
            const auto b1 = e::make_shared<eStadium>(*mBoard, mRotate, mViewedCityId);
            auto &ebs1 = ebs.emplace_back(mHoverTX, mHoverTY, b1);
            ebs1.fBR = e::make_shared<eStadium1Renderer>(b1);
            int dx;
            int dy;
            if (mRotate)
            {
                dx = 0;
                dy = 5;
            }
            else
            {
                dx = 5;
                dy = 0;
            }
            auto &ebs2 = ebs.emplace_back(mHoverTX + dx, mHoverTY + dy, b1);
            ebs2.fBR = e::make_shared<eStadium2Renderer>(b1);
        }
        break;
        case eBuildingMode::bibliotheke:
        {
            const auto b1 = e::make_shared<eBibliotheke>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::observatory:
        {
            const auto b1 = e::make_shared<eObservatory>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::university:
        {
            const auto b1 = e::make_shared<eUniversity>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::laboratory:
        {
            const auto b1 = e::make_shared<eLaboratory>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::inventorsWorkshop:
        {
            const auto b1 = e::make_shared<eInventorsWorkshop>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::museum:
        {
            const auto b1 = e::make_shared<eMuseum>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::palace:
        {
            const int worldTileX = mHoverTX;
            const int worldTileY = mHoverTY;
            const auto b1 = e::make_shared<ePalace>(*mBoard, mRotate, mViewedCityId);

            int dx;
            int dy;
            int tileSpanW;
            int tileSpanH;
            const int tminX = worldTileX - 2;
            const int tminY = worldTileY - 3;
            int tmaxX;
            int tmaxY;
            if (mRotate)
            {
                dx = 0;
                dy = 4;
                tileSpanW = 4;
                tileSpanH = 8;
                tmaxX = tminX + 6;
                tmaxY = tminY + 9;
            }
            else
            {
                dx = 4;
                dy = 0;
                tileSpanW = 8;
                tileSpanH = 4;
                tmaxX = tminX + 9;
                tmaxY = tminY + 6;
            }
            const SDL_Rect rect{tminX + 1, tminY + 1, tileSpanW, tileSpanH};
            for (int x = tminX; x < tmaxX; x++)
            {
                for (int y = tminY; y < tmaxY; y++)
                {
                    const SDL_Point pt{x, y};
                    const bool r = SDL_PointInRect(&pt, &rect);
                    if (r)
                        continue;
                    bool other = x == tminX && y == tminY;
                    if (!other)
                    {
                        if (mRotate)
                        {
                            other = x == tmaxX - 1 && y == tminY;
                        }
                        else
                        {
                            other = x == tminX && y == tmaxY - 1;
                        }
                    }
                    const auto b0 = e::make_shared<ePalaceTile>(
                        *mBoard, other, mViewedCityId);
                    b0->setPalace(b1.get());
                    ebs.emplace_back(x, y, b0);
                }
            }

            auto &ebs1 = ebs.emplace_back(worldTileX, worldTileY, b1);
            ebs1.fBR = e::make_shared<ePalace1Renderer>(b1);
            auto &ebs2 = ebs.emplace_back(worldTileX + dx, worldTileY + dy, b1);
            ebs2.fBR = e::make_shared<ePalace2Renderer>(b1);
            if (dir == eWorldDirection::S)
            {
                std::swap(ebs1, ebs2);
            }
        }
        break;
        case eBuildingMode::eliteHousing:
        {
            int dx1;
            int dy1;
            int dx2;
            int dy2;
            int dx3;
            int dy3;
            int dx4;
            int dy4;
            if (dir == eWorldDirection::N)
            {
                dx1 = 0;
                dy1 = 0;
                dx2 = 2;
                dy2 = 0;
                dx3 = 2;
                dy3 = 2;
                dx4 = 0;
                dy4 = 2;
            }
            else if (dir == eWorldDirection::E)
            {
                dx1 = 2;
                dy1 = 0;
                dx2 = 2;
                dy2 = 2;
                dx3 = 0;
                dy3 = 2;
                dx4 = 0;
                dy4 = 0;
            }
            else if (dir == eWorldDirection::S)
            {
                dx1 = 2;
                dy1 = 2;
                dx2 = 0;
                dy2 = 2;
                dx3 = 0;
                dy3 = 0;
                dx4 = 2;
                dy4 = 0;
            }
            else
            { // if(dir == eWorldDirection::W) {
                dx1 = 0;
                dy1 = 2;
                dx2 = 0;
                dy2 = 0;
                dx3 = 2;
                dy3 = 0;
                dx4 = 2;
                dy4 = 2;
            }
            const auto b1 = e::make_shared<EliteHousing>(*mBoard, mViewedCityId);
            auto &ebs1 = ebs.emplace_back(mHoverTX + dx1, mHoverTY + dy1, b1);
            ebs1.fBR = e::make_shared<eEliteHousingRenderer>(
                eEliteRendererType::top, b1);
            auto &ebs2 = ebs.emplace_back(mHoverTX + dx2, mHoverTY + dy2, b1);
            ebs2.fBR = e::make_shared<eEliteHousingRenderer>(
                eEliteRendererType::right, b1);
            auto &ebs3 = ebs.emplace_back(mHoverTX + dx3, mHoverTY + dy3, b1);
            ebs3.fBR = e::make_shared<eEliteHousingRenderer>(
                eEliteRendererType::bottom, b1);
            auto &ebs4 = ebs.emplace_back(mHoverTX + dx4, mHoverTY + dy4, b1);
            ebs4.fBR = e::make_shared<eEliteHousingRenderer>(
                eEliteRendererType::left, b1);
        }
        break;
        case eBuildingMode::taxOffice:
        {
            const auto b1 = e::make_shared<eTaxOffice>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::mint:
        {
            const auto b1 = e::make_shared<eMint>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::foundry:
        {
            const auto b1 = e::make_shared<eFoundry>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::timberMill:
        {
            const auto b1 = e::make_shared<eTimberMill>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::masonryShop:
        {
            const auto b1 = e::make_shared<eMasonryShop>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::refinery:
        {
            const auto b1 = e::make_shared<eRefinery>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::blackMarbleWorkshop:
        {
            const auto b1 = e::make_shared<eBlackMarbleWorkshop>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;

        case eBuildingMode::oliveTree:
        {
            const auto b1 = e::make_shared<eResourceBuilding>(
                *mBoard, eResourceBuildingType::oliveTree, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
            fertile = true;
        }
        break;
        case eBuildingMode::vine:
        {
            const auto b1 = e::make_shared<eResourceBuilding>(
                *mBoard, eResourceBuildingType::vine, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
            fertile = true;
        }
        break;
        case eBuildingMode::orangeTree:
        {
            const auto b1 = e::make_shared<eResourceBuilding>(
                *mBoard, eResourceBuildingType::orangeTree, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
            fertile = true;
        }
        break;

        case eBuildingMode::huntingLodge:
        {
            const auto b1 = e::make_shared<eHuntingLodge>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;

        case eBuildingMode::corral:
        {
            const auto b1 = e::make_shared<eCorral>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;

        case eBuildingMode::urchinQuay:
        {
            eDiagonalOrientation o = eDiagonalOrientation::topRight;
            canBuildFishery(mHoverTX, mHoverTY, o);
            const auto b1 = e::make_shared<eUrchinQuay>(*mBoard, o, mViewedCityId);
            ebs.emplace_back(worldTileX, worldTileY, b1);
        }
        break;

        case eBuildingMode::fishery:
        {
            eDiagonalOrientation o = eDiagonalOrientation::topRight;
            canBuildFishery(mHoverTX, mHoverTY, o);
            const auto b1 = e::make_shared<eFishery>(*mBoard, o, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;

        case eBuildingMode::triremeWharf:
        {
            eDiagonalOrientation o = eDiagonalOrientation::topRight;
            canBuildTriremeWharf(mHoverTX, mHoverTY, o);
            const auto b1 = e::make_shared<eTriremeWharf>(*mBoard, o, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;

        case eBuildingMode::pier:
        {
            eDiagonalOrientation o = eDiagonalOrientation::topRight;
            canBuildFishery(mHoverTX, mHoverTY, o);
            const auto b1 = e::make_shared<ePier>(*mBoard, o, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
            const int ctid = mGm->tradeCityId();
            const auto &cts = wrld.cities();
            const auto ct = cts[ctid];
            const auto b2 = e::make_shared<TradePost>(
                *mBoard, *ct, mViewedCityId, eTradePostType::pier);
            int worldTileX = mHoverTX;
            int worldTileY = mHoverTY;
            switch (o)
            {
            case eDiagonalOrientation::topRight:
            {
                worldTileY += 3;
            }
            break;
            case eDiagonalOrientation::bottomRight:
            {
                worldTileX -= 3;
            }
            break;
            case eDiagonalOrientation::bottomLeft:
            {
                worldTileY -= 3;
            }
            break;
            default:
            case eDiagonalOrientation::topLeft:
            {
                worldTileX += 3;
            }
            break;
            }
            bool insert = false;
            if (dir == eWorldDirection::N)
            {
                insert = o == eDiagonalOrientation::bottomRight ||
                         o == eDiagonalOrientation::bottomLeft;
            }
            else if (dir == eWorldDirection::E)
            {
                insert = o == eDiagonalOrientation::topLeft ||
                         o == eDiagonalOrientation::bottomLeft;
            }
            else if (dir == eWorldDirection::S)
            {
                insert = o == eDiagonalOrientation::topRight ||
                         o == eDiagonalOrientation::topLeft;
            }
            else
            { // if(dir == eWorldDirection::W) {
                insert = o == eDiagonalOrientation::topRight ||
                         o == eDiagonalOrientation::bottomRight;
            }
            if (insert)
            {
                ebs.insert(ebs.begin(), {worldTileX, worldTileY, b2});
            }
            else
            {
                ebs.emplace_back(worldTileX, worldTileY, b2);
            }
        }
        break;

        case eBuildingMode::wheatFarm:
        {
            const auto b1 = e::make_shared<eWheatFarm>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
            fertile = true;
        }
        break;
        case eBuildingMode::onionFarm:
        {
            const auto b1 = e::make_shared<eOnionFarm>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
            fertile = true;
        }
        break;
        case eBuildingMode::carrotFarm:
        {
            const auto b1 = e::make_shared<eCarrotFarm>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
            fertile = true;
        }
        break;

        case eBuildingMode::growersLodge:
        {
            const auto b1 = e::make_shared<eGrowersLodge>(
                *mBoard, eGrowerType::grapesAndOlives, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::orangeTendersLodge:
        {
            const auto b1 = e::make_shared<eGrowersLodge>(
                *mBoard, eGrowerType::oranges, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;

        case eBuildingMode::granary:
        {
            const auto b1 = e::make_shared<eGranary>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::warehouse:
        {
            const auto b1 = e::make_shared<Warehouse>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;

        case eBuildingMode::tradePost:
        {
            const int ctid = mGm->tradeCityId();
            const auto &cts = wrld.cities();
            const auto ct = cts[ctid];
            const auto b1 = e::make_shared<TradePost>(*mBoard, *ct, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;

        case eBuildingMode::wall:
        {
            const auto b1 = e::make_shared<eWall>(*mBoard, mViewedCityId);
            b1->setDeleteArchers(false);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::tower:
        {
            const auto b1 = e::make_shared<eTower>(*mBoard, mViewedCityId);
            b1->setDeleteArchers(false);
            // check if any tile is on wall
            bool onWall = false;
            for(int dx = 0; dx < 2; dx++) {
                for(int dy = 0; dy < 2; dy++) {
                    const auto t = mBoard->tile(mHoverTX + dx, mHoverTY + dy);
                    if(t) {
                        const auto building = t->underBuilding();
                        if(building && building->type() == eBuildingType::wall) {
                            onWall = true;
                            break;
                        }
                    }
                }
                if(onWall) break;
            }
            if(onWall) {
                const auto tex = b1->getTexture(mTileSize);
                tex->setColorMod(0, 255, 0);
                double drawX, drawY;
                const auto centerTile = mBoard->tile(mHoverTX, mHoverTY);
                if(centerTile) {
                    const int altitude = centerTile->altitude();
                    drawXY(mHoverTX, mHoverTY, drawX, drawY, 2, 2, altitude);
                    tp.drawTexture(drawX, drawY, tex, Alignment::top);
                }
                tex->clearColorMod();
            } else {
                for(int dx = 0; dx < 2; dx++) {
                    for(int dy = 0; dy < 2; dy++) {
                        const auto t = mBoard->tile(mHoverTX + dx, mHoverTY + dy);
                        if(!t) continue;
                        double drawX, drawY;
                        drawXY(mHoverTX + dx, mHoverTY + dy, drawX, drawY, 1, 1, t->altitude());
                        int dd;
                        auto tex = TileToTexture::get(t, trrTexs, builTexs, mTileSize, false, dd, nullptr, eWorldDirection::N);
                        tex->setColorMod(255, 0, 0);
                        tp.drawTexture(drawX, drawY, tex, Alignment::top);
                        tex->clearColorMod();
                    }
                }
            }
        }
        break;
        case eBuildingMode::gatehouse:
        {
            int ddx = 0;
            int ddy = 0;
            int dx;
            int dy;
            if (dir == eWorldDirection::N)
            {
                if (mRotate)
                {
                    dx = 0;
                    dy = 3;
                }
                else
                {
                    dx = 3;
                    dy = 0;
                }
            }
            else if (dir == eWorldDirection::E)
            {
                if (mRotate)
                {
                    dx = 0;
                    dy = 3;
                }
                else
                {
                    dx = 3;
                    dy = 0;
                }
            }
            else if (dir == eWorldDirection::S)
            {
                if (mRotate)
                {
                    dx = 0;
                    dy = -3;
                    ddy = 3;
                }
                else
                {
                    dx = -3;
                    dy = 0;
                    ddx = 3;
                }
            }
            else
            { // if(dir == eWorldDirection::W) {
                if (mRotate)
                {
                    dx = 0;
                    dy = 3;
                }
                else
                {
                    dx = 3;
                    dy = 0;
                }
            }
            const bool switched = (mRotate && dir == eWorldDirection::W) ||
                                  (!mRotate && dir == eWorldDirection::E);
            int x1 = mHoverTX + ddx;
            int y1 = mHoverTY + ddy;
            int x2 = mHoverTX + ddx + dx;
            int y2 = mHoverTY + ddy + dy;
            if (switched)
            {
                std::swap(x1, x2);
                std::swap(y1, y2);
            }
            const auto b1 = e::make_shared<eGatehouse>(*mBoard, mRotate, mViewedCityId);
            auto &ebs1 = ebs.emplace_back(x1, y1, b1);
            ebs1.fBR = e::make_shared<eGatehouseRenderer>(
                mRotate, eGatehouseRendererType::grt1, b1);
            auto &ebs2 = ebs.emplace_back(x2, y2, b1);
            ebs2.fBR = e::make_shared<eGatehouseRenderer>(
                mRotate, eGatehouseRendererType::grt2, b1);
        }
        break;

        case eBuildingMode::armory:
        {
            const auto b1 = e::make_shared<eArmory>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::horseRanch:
        {
            const int worldTileX = mHoverTX;
            const int worldTileY = mHoverTY;
            const auto b1 = e::make_shared<eHorseRanch>(*mBoard, mViewedCityId);
            ebs.emplace_back(worldTileX, worldTileY, b1);

            int dx = 0;
            int dy = 0;
            bool under = false;
            if (mRotateId == 0)
            { // bottomRight
                dx = 3;
            }
            else if (mRotateId == 1)
            { // topRight
                dy = -3;
                dx = -1;
            }
            else if (mRotateId == 2)
            { // topLeft
                dx = -4;
                dy = 1;
                under = true;
            }
            else if (mRotateId == 3)
            { // bottomLeft
                dy = 4;
            }
            if (dir == eWorldDirection::N)
            {
                under = mRotateId == 1 || mRotateId == 2; // topRight, topLeft
            }
            else if (dir == eWorldDirection::E)
            {
                under = mRotateId == 1 || mRotateId == 0; // topLeft, bottomLeft
            }
            else if (dir == eWorldDirection::S)
            {
                under = mRotateId == 0 || mRotateId == 3; // bottomRight, bottomLeft
            }
            else if (dir == eWorldDirection::W)
            {
                under = mRotateId == 2 || mRotateId == 3; // topRight, bottomRight
            }
            const auto b2 = e::make_shared<HorseRanchEnclosure>(*mBoard, mViewedCityId);
            b2->setRanch(b1.get());
            b1->setEnclosure(b2.get());
            if (under)
            {
                ebs.insert(ebs.begin(), eB{worldTileX + dx, worldTileY + dy, b2});
            }
            else
            {
                ebs.emplace_back(worldTileX + dx, worldTileY + dy, b2);
            }
        }
        break;
        case eBuildingMode::chariotFactory:
        {
            const auto b1 = e::make_shared<eChariotFactory>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::olivePress:
        {
            const auto b1 = e::make_shared<eOlivePress>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::winery:
        {
            const auto b1 = e::make_shared<eWinery>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::sculptureStudio:
        {
            const auto b1 = e::make_shared<eSculptureStudio>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;

        case eBuildingMode::artisansGuild:
        {
            const auto b1 = e::make_shared<eArtisansGuild>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;

        case eBuildingMode::dairy:
        {
            const auto b1 = e::make_shared<eDairy>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::cardingShed:
        {
            const auto b1 = e::make_shared<eCardingShed>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;

        case eBuildingMode::foodVendor:
        {
            const auto b1 = e::make_shared<eFoodVendor>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::fleeceVendor:
        {
            const auto b1 = e::make_shared<eFleeceVendor>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::oilVendor:
        {
            const auto b1 = e::make_shared<eOilVendor>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::wineVendor:
        {
            const auto b1 = e::make_shared<eWineVendor>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::armsVendor:
        {
            const auto b1 = e::make_shared<eArmsVendor>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::horseTrainer:
        {
            const auto b1 = e::make_shared<eHorseVendor>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::chariotVendor:
        {
            const auto b1 = e::make_shared<eChariotVendor>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;

        case eBuildingMode::park:
        {
            const auto b1 = e::make_shared<ePark>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::doricColumn:
        {
            const auto b1 = e::make_shared<eDoricColumn>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::ionicColumn:
        {
            const auto b1 = e::make_shared<eIonicColumn>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::corinthianColumn:
        {
            const auto b1 = e::make_shared<eCorinthianColumn>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::avenue:
        {
            const auto b1 = e::make_shared<eAvenue>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;

        case eBuildingMode::populationMonument:
        {
            const auto b1 = e::make_shared<eCommemorative>(0, *mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::victoryMonument:
        {
            const auto b1 = e::make_shared<eCommemorative>(1, *mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::colonyMonument:
        {
            const auto b1 = e::make_shared<eCommemorative>(2, *mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::athleteMonument:
        {
            const auto b1 = e::make_shared<eCommemorative>(3, *mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::conquestMonument:
        {
            const auto b1 = e::make_shared<eCommemorative>(4, *mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::happinessMonument:
        {
            const auto b1 = e::make_shared<eCommemorative>(5, *mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::heroicFigureMonument:
        {
            const auto b1 = e::make_shared<eCommemorative>(6, *mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::diplomacyMonument:
        {
            const auto b1 = e::make_shared<eCommemorative>(7, *mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::scholarMonument:
        {
            const auto b1 = e::make_shared<eCommemorative>(8, *mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;

        case eBuildingMode::aphroditeMonument:
        case eBuildingMode::apolloMonument:
        case eBuildingMode::aresMonument:
        case eBuildingMode::artemisMonument:
        case eBuildingMode::athenaMonument:
        case eBuildingMode::atlasMonument:
        case eBuildingMode::demeterMonument:
        case eBuildingMode::dionysusMonument:
        case eBuildingMode::hadesMonument:
        case eBuildingMode::hephaestusMonument:
        case eBuildingMode::heraMonument:
        case eBuildingMode::hermesMonument:
        case eBuildingMode::poseidonMonument:
        case eBuildingMode::zeusMonument:
        {
            const int worldTileX = mHoverTX;
            const int worldTileY = mHoverTY;
            const int tminX = worldTileX - 1;
            const int tminY = worldTileY - 2;
            const int tmaxX = tminX + 4;
            const int tmaxY = tminY + 4;
            const auto am = eBuildingMode::aphroditeMonument;
            const int id = static_cast<int>(mode) -
                           static_cast<int>(am);
            const auto gt = static_cast<GodType>(id);
            const auto b1 = e::make_shared<eGodMonument>(
                gt, GodQuestId::godQuest1, *mBoard, mViewedCityId);

            for (int x = tminX; x < tmaxX; x++)
            {
                for (int y = tminY; y < tmaxY; y++)
                {
                    const auto b0 = e::make_shared<eGodMonumentTile>(
                        *mBoard, mViewedCityId);
                    b0->setMonument(b1.get());
                    ebs.emplace_back(x, y, b0);
                }
            }

            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;

        case eBuildingMode::bench:
        {
            const auto b1 = e::make_shared<eBench>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::flowerGarden:
        {
            const auto b1 = e::make_shared<eFlowerGarden>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::gazebo:
        {
            const auto b1 = e::make_shared<eGazebo>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::hedgeMaze:
        {
            const auto b1 = e::make_shared<eHedgeMaze>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::fishPond:
        {
            const auto b1 = e::make_shared<eFishPond>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;

        case eBuildingMode::waterPark:
        {
            const auto b1 = e::make_shared<eWaterPark>(*mBoard, mViewedCityId);
            b1->setId(rotationId());
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;

        case eBuildingMode::hippodromePiece:
        {
            updateHippodromeIds();
            const int hid = hippodromeId();
            const auto b1 = e::make_shared<eHippodromePiece>(*mBoard, mViewedCityId);
            b1->setId(hid == -1 ? 0 : hid);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
            if (hid == -1)
            {
                canBuildFunc = [](int, int, int, int)
                { return false; };
            }
        }
        break;

        case eBuildingMode::birdBath:
        {
            const auto b1 = e::make_shared<eBirdBath>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::shortObelisk:
        {
            const auto b1 = e::make_shared<eShortObelisk>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::tallObelisk:
        {
            const auto b1 = e::make_shared<eTallObelisk>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::shellGarden:
        {
            const auto b1 = e::make_shared<eShellGarden>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::sundial:
        {
            const auto b1 = e::make_shared<eSundial>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::orrery:
        {
            const auto b1 = e::make_shared<eOrrery>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::dolphinSculpture:
        {
            const auto b1 = e::make_shared<eDolphinSculpture>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::spring:
        {
            const auto b1 = e::make_shared<eSpring>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::topiary:
        {
            const auto b1 = e::make_shared<eTopiary>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::baths:
        {
            const auto b1 = e::make_shared<eBaths>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        case eBuildingMode::stoneCircle:
        {
            const auto b1 = e::make_shared<eStoneCircle>(*mBoard, mViewedCityId);
            ebs.emplace_back(mHoverTX, mHoverTY, b1);
        }
        break;
        default:
            break;
        }
        const auto drawAppealRangePreview = [&](const eB &eb)
        {
            paintAppealBuildPreview(tp, trrTexs, eb.fB.get(), eb.fBR.get(),
                                    eb.fTx, eb.fTy);
        };
        bool canBuildPreview = true;
        const int previewAltitude = t->altitude();
        for (auto &eb : ebs)
        {
            if (!eb.fBR)
                eb.fBR = e::make_shared<eBuildingRenderer>(eb.fB);
            const auto b = eb.fBR;
            const int tileSpanW = b->spanW();
            const int tileSpanH = b->spanH();
            const bool cb = canBuildFunc(eb.fTx, eb.fTy, tileSpanW, tileSpanH);
            if (!cb)
                canBuildPreview = false;
        }
        const bool isSanctuaryPreview = previewFootprint.w > 0 &&
                                        previewFootprint.h > 0;
        std::stable_sort(ebs.begin(), ebs.end(),
                         [&](const eB &lhs, const eB &rhs)
                         {
                             if (isSanctuaryPreview) {
                                 const SDL_Rect lr{lhs.fTx, lhs.fTy, 1, 1};
                                 const SDL_Rect rr{rhs.fTx, rhs.fTy, 1, 1};
                                 const auto rl = TileHelper::toRotatedRect(lr, dir, boardWidth, boardHeight);
                                 const auto rr2 = TileHelper::toRotatedRect(rr, dir, boardWidth, boardHeight);
                                 if (rl.y != rr2.y) return rl.y < rr2.y;
                                 return rl.x < rr2.x;
                             }
                             const int lhsSpanW = lhs.fBR ? lhs.fBR->spanW() : 1;
                             const int lhsSpanH = lhs.fBR ? lhs.fBR->spanH() : 1;
                             const int rhsSpanW = rhs.fBR ? rhs.fBR->spanW() : 1;
                             const int rhsSpanH = rhs.fBR ? rhs.fBR->spanH() : 1;
                             const SDL_Rect lhsRect{lhs.fTx, lhs.fTy,
                                                    lhsSpanW, lhsSpanH};
                             const SDL_Rect rhsRect{rhs.fTx, rhs.fTy,
                                                    rhsSpanW, rhsSpanH};
                             const auto rotatedLhsRect = TileHelper::toRotatedRect(
                                 lhsRect, dir, boardWidth, boardHeight);
                             const auto rotatedRhsRect = TileHelper::toRotatedRect(
                                 rhsRect, dir, boardWidth, boardHeight);
                             const int lhsY = rotatedLhsRect.y + rotatedLhsRect.h - 1;
                             const int rhsY = rotatedRhsRect.y + rotatedRhsRect.h - 1;
                             if (lhsY != rhsY)
                                 return lhsY < rhsY;
                             const int lhsX = rotatedLhsRect.x + rotatedLhsRect.w - 1;
                             const int rhsX = rotatedRhsRect.x + rotatedRhsRect.w - 1;
                             return lhsX < rhsX;
                         });
        for (const auto &eb : ebs)
        {
            drawAppealRangePreview(eb);
        }
        if (previewFootprint.w > 0 && previewFootprint.h > 0)
        {
            drawSanctuaryTerrainPreview(
                *mBoard, tp, builTexs, trrTexs,
                mode, mRotateId, mHoverTX, mHoverTY, mViewedCityId,
                previewFootprint, dir, boardWidth, boardHeight, mAnimFrame,
                canBuildPreview);
        }
        if (isSanctuaryPreview)
        {
            for (auto &eb : ebs)
            {
                if (!eb.fB || !eb.fBR)
                    continue;
                if (eb.fTorch)
                {
                    drawSanctuaryTorchPreview(
                        *mBoard, tp, builTexs, eb.fTx, eb.fTy,
                        eb.fAltitude, mAnimFrame);
                    continue;
                }
                const auto type = eb.fB->type();
                if (type == eBuildingType::templeTile)
                    continue;
                if (type == eBuildingType::temple)
                {
                    const auto temple =
                        dynamic_cast<eTempleBuilding*>(eb.fB.get());
                    const bool longTemple =
                        isSanctuaryLongTempleGod(eb.fStatueGod);
                    drawSanctuaryTempleBuildingPreview(
                        *mBoard, tp, builTexs, eb.fTx, eb.fTy,
                        eb.fAltitude, eb.fTempleOverlayDirId,
                        dir, mAnimFrame, canBuildPreview, 0,
                        temple ? temple->id() : -1, longTemple);
                    continue;
                }
                if (type == eBuildingType::templeStatue)
                {
                    drawSanctuaryStatuePreview(
                        *mBoard, tp, builTexs, eb.fStatueGod,
                        eb.fStatueTextureId, eb.fTx, eb.fTy,
                        eb.fAltitude, dir, canBuildPreview);
                    continue;
                }
                if (type == eBuildingType::templeMonument)
                {
                    drawSanctuaryMonumentPreview(
                        *mBoard, tp, builTexs, eb.fStatueGod,
                        eb.fMonumentTextureId, eb.fTx, eb.fTy,
                        eb.fAltitude, dir, canBuildPreview);
                    continue;
                }
                if (type == eBuildingType::templeAltar)
                {
                    drawSanctuaryAltarPreview(
                        *mBoard, tp, builTexs, eb.fTx, eb.fTy,
                        eb.fAltitude, dir, mRotateId, canBuildPreview);
                    continue;
                }
                drawGenericBuildPreviewPart(
                    tp, eb.fB.get(), eb.fBR.get(), t, eb.fTx, eb.fTy,
                    eb.fAltitude, dir, canBuildPreview);
            }
        }
        else
        {
            for (auto &eb : ebs)
            {
                if (!eb.fB || !eb.fBR)
                    continue;
                drawGenericBuildPreviewPart(
                    tp, eb.fB.get(), eb.fBR.get(), t, eb.fTx, eb.fTy,
                    previewAltitude, dir, canBuildPreview);
            }
        }
        drawRoadAccessPreview(ebs, canBuildPreview);
    }
}
