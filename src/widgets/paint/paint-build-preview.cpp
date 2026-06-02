#include "widgets/game-widget.h"

#include "characters/actions/walkable/ewalkableobject.h"
#include "engine/etile.h"
#include "engine/game-board.h"
#include "enumbers.h"
#include "evectorhelpers.h"
#include "widgets/etilepainter.h"
#include "widgets/epainter.h"
#include "textures/eterraintextures.h"
#include "textures/ebuildingtextures.h"
#include "textures/egametextures.h"
#include "buildings/allbuildings.h"
#include "buildings/ebuildingrenderer.h"
#include "buildings/eagorabase.h"
#include "buildings/eagoraspace.h"
#include "etilehelper.h"
#include "textures/etiletotexture.h"
#include "widgets/gamebuild/ecommonhousingbuild.h"
#include "widgets/paint/build-preview-render.h"
#include "widgets/paint/sanctuary-preview.h"

#include <algorithm>
#include <set>

void GameWidget::paintBuildPreview(
    eTilePainter& tp,
    ePainter& painter,
    const eTerrainTextures& trrTexs,
    const eBuildingTextures& builTexs,
    ePlayerId ppid,
    eBuildingMode mode,
    eWorldDirection dir,
    int boardw, int boardh,
    bool bridgeValid,
    const std::vector<eTile*>& bridgetTs,
    int sMinX, int sMaxX, int sMinY, int sMaxY)
{
    const auto drawBuildText = [&](const std::string &text)
    {
        painter.drawText(mHoverX - mDX + padding(), mHoverY - mDY + padding(), text, eFontColor::light);
    };

    const auto drawStampCostEstimate = [&]()
    {
        const auto diff = mBoard->difficulty(ppid);
        painter.drawText(mHoverX - mDX + padding(),
                   mHoverY - mDY + padding(),
                   std::to_string(mStampTool->estimatedCost(diff)),
                   eFontColor::red,
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
            double rx;
            double ry;
            drawXY(tile->x(), tile->y(), rx, ry, 1, 1, tile->altitude());
            tp.drawTexture(rx, ry, tex, eAlignment::top);
        }
        tex->clearAlphaMod();
        tex->clearColorMod();
    };
    if(mCreatingStampTemplate) drawStampTemplateSelection();

    const auto animalBuildTexture = [&](const eBuildingMode mode) {
        auto& charTexs = eGameTextures::characters()[static_cast<int>(mTileSize)];
        const eAnimalTextures* animalTexs = nullptr;
        const eCattleTextures* cattleTexs = nullptr;
        switch(mode) {
        case eBuildingMode::sheep:
            eGameTextures::loadSheep();
            animalTexs = &charTexs.fNudeSheep;
            break;
        case eBuildingMode::goat:
            eGameTextures::loadGoat();
            animalTexs = &charTexs.fGoat;
            break;
        case eBuildingMode::cattle:
            eGameTextures::loadCattle();
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
        return std::shared_ptr<eTexture>{};
    };
    const auto drawAnimalBuildGhost = [&](eTile* const tile,
                                          const bool valid) {
        if(!tile) return;
        const auto tex = animalBuildTexture(mode);
        if(!tex) return;
        const int tx = tile->x();
        const int ty = tile->y();
        int rtx;
        int rty;
        eTileHelper::tileIdToRotatedTileId(tx, ty,
                                           rtx, rty, dir,
                                           boardw, boardh);
        const int da = tile->characterDoubleAltitude();
        double x;
        double y;
        if(dir == eWorldDirection::N) {
            x = tx - da*0.5 + 0.75;
            y = ty - da*0.5 + 0.75;
        } else if(dir == eWorldDirection::E) {
            x = rtx - da*0.5 + 0.75;
            y = rty - da*0.5 + 0.75;
        } else if(dir == eWorldDirection::S) {
            x = rtx - da*0.5 + 0.75;
            y = rty - da*0.5 + 0.75;
        } else {
            x = rtx - da*0.5 + 0.75;
            y = rty - da*0.5 + 0.75;
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
    const int tx = mHoverTX;
    const int ty = mHoverTY;
    const int a = t ? t->altitude() : 0;
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
            double rx;
            double ry;
            drawXY(t->x(), t->y(), rx, ry, 1, 1, t->altitude());
            tp.drawTexture(rx, ry, tex, eAlignment::top);

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
            eGameTextures::loadBridge();
            const auto hoverTile = mBoard->tile(mHoverTX, mHoverTY);
            if (hoverTile)
            {
                const auto &tex = builTexs.fBridge.getTexture(10);
                tex->setColorMod(255, 0, 0);
                double rx;
                double ry;
                const int hx = hoverTile->x();
                const int hy = hoverTile->y();
                const int ha = hoverTile->altitude();
                drawXY(hx, hy, rx, ry, 1, 1, ha);
                tp.drawTexture(rx, ry, tex, eAlignment::top);
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
            std::shared_ptr<eTexture> tex;
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
                    double rx;
                    double ry;
                    const auto t = mBoard->tile(x, y);
                    if (!t)
                        continue;
                    if (t->terrain() != eTerrain::fertile)
                        continue;
                    if (t->underBuilding())
                        continue;
                    const int a = t->altitude();
                    drawXY(x, y, rx, ry, 1, 1, a);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);

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
            const auto bt = eBuildingModeHelpers::toBuildingType(mode);
            const int allowed = mBoard->countAllowed(mViewedCityId, bt);
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
                    double rx;
                    double ry;
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
                    const int a = t->altitude();
                    drawXY(x, y, rx, ry, 1, 1, a);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);

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
                double rx;
                double ry;
                const auto t = mBoard->tile(rect.fX, rect.fY);
                if (!t)
                    continue;
                const int a = t->altitude();
                drawXY(rect.fX, rect.fY, rx, ry, 1, 1, a);
                tp.drawTexture(rx, ry, tex, eAlignment::top);
                tp.drawTexture(rx + 1, ry, tex, eAlignment::top);
                tp.drawTexture(rx, ry + 1, tex, eAlignment::top);
                tp.drawTexture(rx + 1, ry + 1, tex, eAlignment::top);
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
        const auto bt = eBuildingModeHelpers::toBuildingType(mode);
        const int allowed = mBoard->countAllowed(mViewedCityId, bt);
        const auto t = mBoard->tile(mHoverTX, mHoverTY);
        if (!t)
            return;
        const int tx = t->x();
        const int ty = t->y();
        const int animalW = 1;
        const int animalH = 1;
        const bool cb = allowed > 0 && mBoard->canBuild(
                                           tx, ty, animalW, animalH,
                                           mEditorMode,
                                           mViewedCityId, ppid,
                                           true, true);
        drawAnimalBuildGhost(t, cb);
        return;
    }

    if (mode == eBuildingMode::stamp)
    {
        paintStampPreview(tp, trrTexs, builTexs, tx, ty, ppid);
        drawStampCostEstimate();
        return;
    }

    switch (mode)
    {
    case eBuildingMode::commonAgora:
    {
        eGameTextures::loadAgora();
        const auto &agr = builTexs.fAgora;
        const auto &agrr = builTexs.fAgoraRoad;

        eAgoraOrientation bt;
        const auto p = agoraBuildPlaceIter(t, false, bt, mViewedCityId, ppid);
        if (p.empty())
        {
            const auto &tex = trrTexs.fBuildingBase;
            tex->setColorMod(255, 0, 0);
            for (int i = tx - 1; i < tx + 2; i++)
            {
                for (int j = ty - 3; j < ty + 3; j++)
                {
                    double rx;
                    double ry;
                    drawXY(i, j, rx, ry, 1, 1, a);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
                }
            }
            tex->clearColorMod();
            const int texId = (dir == eWorldDirection::N ||
                               dir == eWorldDirection::S)
                                  ? 0
                                  : 1;
            const auto &road = trrTexs.fRoad.getTexture(texId);
            road->setColorMod(255, 0, 0);
            for (int j = ty - 3; j < ty + 3; j++)
            {
                double rx;
                double ry;
                drawXY(tx - 1, j, rx, ry, 1, 1, a);
                tp.drawTexture(rx, ry, road, eAlignment::top);
            }
            road->clearColorMod();
        }
        else
        {
            drawAgoraRoadAccessPreview(p);
            if (bt == eAgoraOrientation::bottomRight)
            {
                const int iMax = p.size();
                for (int i = 0; i < iMax; i++)
                {
                    const auto t = p[i];
                    stdsptr<eTexture> tex;
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
                    double rx;
                    double ry;
                    const int tx = t->x();
                    const int ty = t->y();
                    const int a = t->altitude();
                    drawXY(tx, ty, rx, ry, dim, dim, a);
                    if (dim == 2)
                    {
                        if (dir == eWorldDirection::E)
                        {
                            rx -= 1;
                        }
                        else if (dir == eWorldDirection::S)
                        {
                            rx -= 1;
                            ry += 1;
                        }
                        else if (dir == eWorldDirection::W)
                        {
                            ry += 1;
                        }
                    }
                    tex->setColorMod(0, 255, 0);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
                    tex->clearColorMod();
                }
            }
            else if (bt == eAgoraOrientation::topLeft)
            {
                const int iMax = p.size();
                for (int i = 0; i < iMax; i++)
                {
                    const auto t = p[i];
                    stdsptr<eTexture> tex;
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
                    double rx;
                    double ry;
                    const int tx = t->x();
                    const int ty = t->y();
                    const int a = t->altitude();
                    drawXY(tx, ty, rx, ry, dim, dim, a);
                    if (dim == 2)
                    {
                        if (dir == eWorldDirection::E)
                        {
                            rx -= 1;
                        }
                        else if (dir == eWorldDirection::S)
                        {
                            rx -= 1;
                            ry += 1;
                        }
                        else if (dir == eWorldDirection::W)
                        {
                            ry += 1;
                        }
                    }
                    tex->setColorMod(0, 255, 0);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
                    tex->clearColorMod();
                }
            }
            else if (bt == eAgoraOrientation::bottomLeft)
            {
                const int iMax = p.size();
                for (int i = 0; i < iMax; i++)
                {
                    const auto t = p[i];
                    stdsptr<eTexture> tex;
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
                    double rx;
                    double ry;
                    const int tx = t->x();
                    const int ty = t->y();
                    const int a = t->altitude();
                    drawXY(tx, ty, rx, ry, dim, dim, a);
                    if (dim == 2)
                    {
                        if (dir == eWorldDirection::E)
                        {
                            rx -= 1;
                        }
                        else if (dir == eWorldDirection::S)
                        {
                            rx -= 1;
                            ry += 1;
                        }
                        else if (dir == eWorldDirection::W)
                        {
                            ry += 1;
                        }
                    }
                    tex->setColorMod(0, 255, 0);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
                    tex->clearColorMod();
                }
            }
            else if (bt == eAgoraOrientation::topRight)
            {
                const int iMax = p.size();
                for (int i = 0; i < iMax; i++)
                {
                    const auto t = p[i];
                    stdsptr<eTexture> tex;
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
                    double rx;
                    double ry;
                    const int tx = t->x();
                    const int ty = t->y();
                    const int a = t->altitude();
                    drawXY(tx, ty, rx, ry, dim, dim, a);
                    if (dim == 2)
                    {
                        if (dir == eWorldDirection::E)
                        {
                            rx -= 1;
                        }
                        else if (dir == eWorldDirection::S)
                        {
                            rx -= 1;
                            ry += 1;
                        }
                        else if (dir == eWorldDirection::W)
                        {
                            ry += 1;
                        }
                    }
                    tex->setColorMod(0, 255, 0);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
                    tex->clearColorMod();
                }
            }
        }
    }
    break;
    case eBuildingMode::grandAgora:
    {
        eGameTextures::loadAgora();
        const auto &agr = builTexs.fAgora;
        const auto &agrr = builTexs.fAgoraRoad;

        eAgoraOrientation bt;
        const auto p = agoraBuildPlaceIter(t, true, bt, mViewedCityId, ppid);
        if (p.empty())
        {
            const auto &tex = trrTexs.fBuildingBase;
            tex->setColorMod(255, 0, 0);
            for (int i = tx - 2; i < tx + 3; i++)
            {
                for (int j = ty - 3; j < ty + 3; j++)
                {
                    double rx;
                    double ry;
                    drawXY(i, j, rx, ry, 1, 1, a);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
                }
            }
            tex->clearColorMod();
            const int texId = (dir == eWorldDirection::N ||
                               dir == eWorldDirection::S)
                                  ? 0
                                  : 1;
            const auto &road = trrTexs.fRoad.getTexture(texId);
            road->setColorMod(255, 0, 0);
            for (int j = ty - 3; j < ty + 3; j++)
            {
                double rx;
                double ry;
                drawXY(tx, j, rx, ry, 1, 1, a);
                tp.drawTexture(rx, ry, road, eAlignment::top);
            }
            road->clearColorMod();
        }
        else
        {
            drawAgoraRoadAccessPreview(p);
            if (bt == eAgoraOrientation::bottomRight)
            {
                const int iMax = p.size();
                for (int i = 0; i < iMax; i++)
                {
                    const auto t = p[i];
                    const int tx = t->x();
                    const int ty = t->y();
                    const int a = t->altitude();
                    stdsptr<eTexture> tex;
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
                    double rx;
                    double ry;
                    drawXY(tx, ty, rx, ry, dim, dim, a);
                    if (dim == 2)
                    {
                        if (dir == eWorldDirection::E)
                        {
                            rx -= 1;
                        }
                        else if (dir == eWorldDirection::S)
                        {
                            rx -= 1;
                            ry += 1;
                        }
                        else if (dir == eWorldDirection::W)
                        {
                            ry += 1;
                        }
                    }
                    tex->setColorMod(0, 255, 0);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
                    tex->clearColorMod();
                }
            }
            else if (bt == eAgoraOrientation::bottomLeft)
            {
                const int iMax = p.size();
                for (int i = 0; i < iMax; i++)
                {
                    const auto t = p[i];
                    const int tx = t->x();
                    const int ty = t->y();
                    const int a = t->altitude();
                    stdsptr<eTexture> tex;
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
                    double rx;
                    double ry;
                    drawXY(tx, ty, rx, ry, dim, dim, a);
                    if (dim == 2)
                    {
                        if (dir == eWorldDirection::E)
                        {
                            rx -= 1;
                        }
                        else if (dir == eWorldDirection::S)
                        {
                            rx -= 1;
                            ry += 1;
                        }
                        else if (dir == eWorldDirection::W)
                        {
                            ry += 1;
                        }
                    }
                    tex->setColorMod(0, 255, 0);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
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
        std::function<bool(const int tx, const int ty,
                           const int sw, const int sh)>
            canBuildFunc;
        switch (mode)
        {
        case eBuildingMode::urchinQuay:
        case eBuildingMode::fishery:
        {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh)
            {
                (void)sw;
                (void)sh;
                eDiagonalOrientation o;
                return canBuildFishery(tx, ty, o);
            };
        }
        break;
        case eBuildingMode::triremeWharf:
        {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh)
            {
                (void)sw;
                (void)sh;
                eDiagonalOrientation o;
                return canBuildTriremeWharf(tx, ty, o);
            };
        }
        break;
        case eBuildingMode::pier:
        {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh)
            {
                if (sw > 2 || sh > 2)
                    return true;
                eDiagonalOrientation o;
                return canBuildPier(tx, ty, o, mViewedCityId, ppid, mEditorMode);
            };
        }
        break;
        case eBuildingMode::palace:
        {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh)
            {
                if (mBoard->hasPalace(mViewedCityId))
                    return false;
                return mBoard->canBuild(tx, ty, sw, sh,
                                        mEditorMode,
                                        mViewedCityId, ppid,
                                        fertile);
            };
        }
        break;
        case eBuildingMode::stadium:
        {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh)
            {
                if (mBoard->hasStadium(mViewedCityId))
                    return false;
                return mBoard->canBuild(tx, ty, sw, sh,
                                        mEditorMode,
                                        mViewedCityId, ppid,
                                        fertile);
            };
        }
        break;
        case eBuildingMode::foodVendor:
        {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh)
            {
                (void)sw;
                (void)sh;
                return canBuildVendor(tx, ty, eResourceType::food);
            };
        }
        break;
        case eBuildingMode::fleeceVendor:
        {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh)
            {
                (void)sw;
                (void)sh;
                return canBuildVendor(tx, ty, eResourceType::fleece);
            };
        }
        break;
        case eBuildingMode::oilVendor:
        {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh)
            {
                (void)sw;
                (void)sh;
                return canBuildVendor(tx, ty, eResourceType::oliveOil);
            };
        }
        break;
        case eBuildingMode::wineVendor:
        {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh)
            {
                (void)sw;
                (void)sh;
                return canBuildVendor(tx, ty, eResourceType::wine);
            };
        }
        break;
        case eBuildingMode::armsVendor:
        {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh)
            {
                (void)sw;
                (void)sh;
                return canBuildVendor(tx, ty, eResourceType::armor);
            };
        }
        break;
        case eBuildingMode::horseTrainer:
        {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh)
            {
                (void)sw;
                (void)sh;
                return canBuildVendor(tx, ty, eResourceType::horse);
            };
        }
        break;
        case eBuildingMode::chariotVendor:
        {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh)
            {
                (void)sw;
                (void)sh;
                return canBuildVendor(tx, ty, eResourceType::chariot);
            };
        }
        break;
        case eBuildingMode::avenue:
        {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh)
            {
                (void)sw;
                (void)sh;
                const auto t = mBoard->tile(tx, ty);
                return canBuildAvenue(t, mViewedCityId, ppid, mEditorMode);
            };
        }
        break;
        default:
        {
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh)
            {
                return mBoard->canBuild(tx, ty, sw, sh,
                                        mEditorMode,
                                        mViewedCityId, ppid,
                                        fertile);
            };
        }
        break;
        }

        struct eB
        {
            eB(const int tx, const int ty,
               const stdsptr<eBuilding> &b) : fTx(tx), fTy(ty), fB(b) {}

            int fTx;
            int fTy;
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
                const int sw = eb.fBR->spanW();
                const int sh = eb.fBR->spanH();
                int minX;
                int minY;
                int maxX;
                int maxY;
                GameBoard::sBuildTiles(minX, minY, maxX, maxY,
                                        eb.fTx, eb.fTy, sw, sh);
                pb->setTileRect({minX, minY, sw, sh});
                const auto roads = pb->surroundingRoad(false, true);
                drawRoadBands(roads, tp, trrTexs);
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
            int sw;
            int sh;
            ePyramid::sDimensions(type, sw, sh);
            const int xMin = mHoverTX - sw / 2;
            const int yMin = mHoverTY - sh / 2;
            const int xMax = xMin + sw;
            const int yMax = yMin + sh;
            const bool cb = mBoard->canBuildBase(xMin, xMax, yMin, yMax,
                                                 mEditorMode,
                                                 mViewedCityId, ppid);
            if (!cb)
                tex->setColorMod(255, 0, 0);
            for (int x = xMin; x < xMax; x++)
            {
                for (int y = yMin; y < yMax; y++)
                {
                    double rx;
                    double ry;
                    const auto t = mBoard->tile(x, y);
                    if (!t)
                        continue;
                    if (t->underBuilding())
                        continue;
                    const int a = t->altitude();
                    drawXY(x, y, rx, ry, 1, 1, a);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
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
            const auto type = eBuildingModeHelpers::toBuildingType(mode);
            const auto god = static_cast<eGodType>(
                static_cast<int>(mode) -
                static_cast<int>(eBuildingMode::templeAphrodite));
            const auto h = eSanctBlueprints::sSanctuaryBlueprint(type, mRotate);
            const int sw = h->fW;
            const int sh = h->fH;
            const int xMin = mHoverTX - sw / 2;
            const int yMin = mHoverTY - sh / 2;
            const auto sanctuary = e::make_shared<eMonument>(
                *mBoard, type, sw, sh, 0, mViewedCityId);
            sanctuary->setRotated(mRotate);
            sanctuary->setTileRect({xMin, yMin, sw, sh});
            eGameTextures::loadZeusSanctuary();
            eSanctuary::sLoadMonumentTextures(god);
            eGameTextures::loadSanctuary();
            const int d = mRotate ? 1 : 0;
            const auto finishSanctuaryPart = [](eSanctBuilding* const part)
            {
                while (!part->finished())
                    part->incProgress();
            };
            for (const auto &tv : h->fTiles)
            {
                for (const auto &te : tv)
                {
                    const int tx = xMin + te.fX;
                    const int ty = yMin + te.fY;
                    switch (te.fType)
                    {
                    case eSanctEleType::tile:
                    case eSanctEleType::copper:
                    case eSanctEleType::silver:
                    case eSanctEleType::oliveTree:
                    case eSanctEleType::vine:
                    case eSanctEleType::orangeTree:
                    {
                        const auto b1 = e::make_shared<eTempleTileBuilding>(
                            te.fId, *mBoard, mViewedCityId);
                        b1->setMonument(sanctuary.get());
                        finishSanctuaryPart(b1.get());
                        sanctuary->registerElement(b1);
                        ebs.emplace_back(tx, ty, b1);
                    }
                    break;
                    case eSanctEleType::sanctuary:
                    {
                        const int stx = mRotate ? tx - 2 : tx + 1;
                        const int sty = mRotate ? ty + 2 : ty - 1;
                        const auto b1 = e::make_shared<eTempleBuilding>(
                            te.fId, *mBoard, mViewedCityId);
                        b1->setMonument(sanctuary.get());
                        finishSanctuaryPart(b1.get());
                        sanctuary->registerElement(b1);
                        ebs.emplace_back(stx, sty, b1);
                    }
                    break;
                    case eSanctEleType::monument:
                    {
                        const auto b1 = e::make_shared<eTempleMonumentBuilding>(
                            god, te.fId, *mBoard, mViewedCityId);
                        b1->setMonument(sanctuary.get());
                        finishSanctuaryPart(b1.get());
                        sanctuary->registerElement(b1);
                        ebs.emplace_back(tx - d, ty + d, b1);
                    }
                    break;
                    case eSanctEleType::altar:
                    {
                        const auto b1 = e::make_shared<eTempleAltarBuilding>(
                            *mBoard, mViewedCityId);
                        b1->setMonument(sanctuary.get());
                        finishSanctuaryPart(b1.get());
                        sanctuary->registerElement(b1);
                        ebs.emplace_back(tx - d, ty + d, b1);
                    }
                    break;
                    case eSanctEleType::defaultStatue:
                    case eSanctEleType::aphroditeStatue:
                    case eSanctEleType::apolloStatue:
                    case eSanctEleType::aresStatue:
                    case eSanctEleType::artemisStatue:
                    case eSanctEleType::athenaStatue:
                    case eSanctEleType::atlasStatue:
                    case eSanctEleType::demeterStatue:
                    case eSanctEleType::dionysusStatue:
                    case eSanctEleType::hadesStatue:
                    case eSanctEleType::hephaestusStatue:
                    case eSanctEleType::heraStatue:
                    case eSanctEleType::hermesStatue:
                    case eSanctEleType::poseidonStatue:
                    case eSanctEleType::zeusStatue:
                    {
                        eGodType statueType;
                        switch (te.fType)
                        {
                        case eSanctEleType::aphroditeStatue: statueType = eGodType::aphrodite; break;
                        case eSanctEleType::apolloStatue:    statueType = eGodType::apollo;    break;
                        case eSanctEleType::aresStatue:      statueType = eGodType::ares;      break;
                        case eSanctEleType::artemisStatue:   statueType = eGodType::artemis;   break;
                        case eSanctEleType::athenaStatue:    statueType = eGodType::athena;    break;
                        case eSanctEleType::atlasStatue:     statueType = eGodType::atlas;     break;
                        case eSanctEleType::demeterStatue:   statueType = eGodType::demeter;   break;
                        case eSanctEleType::dionysusStatue:  statueType = eGodType::dionysus;  break;
                        case eSanctEleType::hadesStatue:     statueType = eGodType::hades;     break;
                        case eSanctEleType::hephaestusStatue: statueType = eGodType::hephaestus; break;
                        case eSanctEleType::heraStatue:      statueType = eGodType::hera;      break;
                        case eSanctEleType::hermesStatue:    statueType = eGodType::hermes;    break;
                        case eSanctEleType::poseidonStatue:  statueType = eGodType::poseidon;  break;
                        case eSanctEleType::zeusStatue:      statueType = eGodType::zeus;      break;
                        default:                             statueType = god;                 break;
                        }
                        const auto b1 = e::make_shared<eTempleStatueBuilding>(
                            statueType, te.fId, *mBoard, mViewedCityId);
                        b1->setMonument(sanctuary.get());
                        finishSanctuaryPart(b1.get());
                        sanctuary->registerElement(b1);
                        ebs.emplace_back(tx, ty, b1);
                    }
                    break;
                    default:
                        break;
                    }
                }
            }
            canBuildFunc = [&, xMin, yMin, sw, sh](
                               const int, const int, const int, const int)
            {
                return mBoard->canBuildBase(xMin, xMin + sw, yMin, yMin + sh,
                                            mEditorMode, mViewedCityId, ppid);
            };
            previewFootprint = {xMin, yMin, sw, sh};
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
            canBuildFunc = [&](const int tx, const int ty,
                               const int sw, const int sh)
            {
                (void)sw;
                (void)sh;
                const auto t = mBoard->tile(tx, ty);
                if (!t)
                    return false;
                const bool hr = t->hasRoad();
                if (!hr)
                    return false;
                const bool b = t->hasBridge();
                if (b)
                    return false;
                const auto ub = t->underBuilding();
                const auto r = static_cast<eRoad *>(ub);
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
            const int tx = mHoverTX;
            const int ty = mHoverTY;
            const auto b1 = e::make_shared<ePalace>(*mBoard, mRotate, mViewedCityId);

            int dx;
            int dy;
            int sw;
            int sh;
            const int tminX = tx - 2;
            const int tminY = ty - 3;
            int tmaxX;
            int tmaxY;
            if (mRotate)
            {
                dx = 0;
                dy = 4;
                sw = 4;
                sh = 8;
                tmaxX = tminX + 6;
                tmaxY = tminY + 9;
            }
            else
            {
                dx = 4;
                dy = 0;
                sw = 8;
                sh = 4;
                tmaxX = tminX + 9;
                tmaxY = tminY + 6;
            }
            const SDL_Rect rect{tminX + 1, tminY + 1, sw, sh};
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

            auto &ebs1 = ebs.emplace_back(tx, ty, b1);
            ebs1.fBR = e::make_shared<ePalace1Renderer>(b1);
            auto &ebs2 = ebs.emplace_back(tx + dx, ty + dy, b1);
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
            ebs.emplace_back(tx, ty, b1);
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
            int tx = mHoverTX;
            int ty = mHoverTY;
            switch (o)
            {
            case eDiagonalOrientation::topRight:
            {
                ty += 3;
            }
            break;
            case eDiagonalOrientation::bottomRight:
            {
                tx -= 3;
            }
            break;
            case eDiagonalOrientation::bottomLeft:
            {
                ty -= 3;
            }
            break;
            default:
            case eDiagonalOrientation::topLeft:
            {
                tx += 3;
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
                ebs.insert(ebs.begin(), {tx, ty, b2});
            }
            else
            {
                ebs.emplace_back(tx, ty, b2);
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
                        const auto ub = t->underBuilding();
                        if(ub && ub->type() == eBuildingType::wall) {
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
                double rx, ry;
                const auto centerTile = mBoard->tile(mHoverTX, mHoverTY);
                if(centerTile) {
                    const int a = centerTile->altitude();
                    drawXY(mHoverTX, mHoverTY, rx, ry, 2, 2, a);
                    tp.drawTexture(rx, ry, tex, eAlignment::top);
                }
                tex->clearColorMod();
            } else {
                for(int dx = 0; dx < 2; dx++) {
                    for(int dy = 0; dy < 2; dy++) {
                        const auto t = mBoard->tile(mHoverTX + dx, mHoverTY + dy);
                        if(!t) continue;
                        double rx, ry;
                        drawXY(mHoverTX + dx, mHoverTY + dy, rx, ry, 1, 1, t->altitude());
                        int dd;
                        auto tex = eTileToTexture::get(t, trrTexs, builTexs, mTileSize, false, dd, nullptr, eWorldDirection::N);
                        tex->setColorMod(255, 0, 0);
                        tp.drawTexture(rx, ry, tex, eAlignment::top);
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
            const int tx = mHoverTX;
            const int ty = mHoverTY;
            const auto b1 = e::make_shared<eHorseRanch>(*mBoard, mViewedCityId);
            ebs.emplace_back(tx, ty, b1);

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
            const auto b2 = e::make_shared<eHorseRanchEnclosure>(*mBoard, mViewedCityId);
            b2->setRanch(b1.get());
            b1->setEnclosure(b2.get());
            if (under)
            {
                ebs.insert(ebs.begin(), eB{tx + dx, ty + dy, b2});
            }
            else
            {
                ebs.emplace_back(tx + dx, ty + dy, b2);
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
            const int tx = mHoverTX;
            const int ty = mHoverTY;
            const int tminX = tx - 1;
            const int tminY = ty - 2;
            const int tmaxX = tminX + 4;
            const int tmaxY = tminY + 4;
            const auto am = eBuildingMode::aphroditeMonument;
            const int id = static_cast<int>(mode) -
                           static_cast<int>(am);
            const auto gt = static_cast<eGodType>(id);
            const auto b1 = e::make_shared<eGodMonument>(
                gt, eGodQuestId::godQuest1, *mBoard, mViewedCityId);

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
            const int sw = b->spanW();
            const int sh = b->spanH();
            const bool cb = canBuildFunc(eb.fTx, eb.fTy, sw, sh);
            if (!cb)
                canBuildPreview = false;
        }
        const bool isSanctuaryPreview = previewFootprint.w > 0 &&
                                        previewFootprint.h > 0;
        std::stable_sort(ebs.begin(), ebs.end(),
                         [&](const eB &lhs, const eB &rhs)
                         {
                             const int lhsSpanW = lhs.fBR ? lhs.fBR->spanW() : 1;
                             const int lhsSpanH = lhs.fBR ? lhs.fBR->spanH() : 1;
                             const int rhsSpanW = rhs.fBR ? rhs.fBR->spanW() : 1;
                             const int rhsSpanH = rhs.fBR ? rhs.fBR->spanH() : 1;
                             const SDL_Rect lhsRect{lhs.fTx, lhs.fTy,
                                                    lhsSpanW, lhsSpanH};
                             const SDL_Rect rhsRect{rhs.fTx, rhs.fTy,
                                                    rhsSpanW, rhsSpanH};
                             const auto rotatedLhsRect = eTileHelper::toRotatedRect(
                                 lhsRect, dir, boardw, boardh);
                             const auto rotatedRhsRect = eTileHelper::toRotatedRect(
                                 rhsRect, dir, boardw, boardh);
                             if (isSanctuaryPreview)
                             {
                                 if (rotatedLhsRect.y != rotatedRhsRect.y)
                                     return rotatedLhsRect.y < rotatedRhsRect.y;
                                 if (rotatedLhsRect.x != rotatedRhsRect.x)
                                     return rotatedLhsRect.x < rotatedRhsRect.x;
                                 return (lhsSpanW + lhsSpanH) <
                                        (rhsSpanW + rhsSpanH);
                             }
                             const int lhsDrawOrder =
                                 (rotatedLhsRect.x + rotatedLhsRect.w - 1) +
                                 (rotatedLhsRect.y + rotatedLhsRect.h - 1);
                             const int rhsDrawOrder =
                                 (rotatedRhsRect.x + rotatedRhsRect.w - 1) +
                                 (rotatedRhsRect.y + rotatedRhsRect.h - 1);
                             if (lhsDrawOrder != rhsDrawOrder)
                                 return lhsDrawOrder < rhsDrawOrder;
                             return (lhsSpanW + lhsSpanH) <
                                    (rhsSpanW + rhsSpanH);
                         });
        for (const auto &eb : ebs)
        {
            drawAppealRangePreview(eb);
        }
        if (previewFootprint.w > 0 && previewFootprint.h > 0)
        {
            drawSanctuaryTerrainPreview(
                *mBoard, tp, trrTexs, previewFootprint,
                dir, boardw, boardh, canBuildPreview);
        }
        if (isSanctuaryPreview)
        {
            for (auto &eb : ebs)
            {
                if (!eb.fB || !eb.fBR)
                    continue;
                const auto type = eb.fB->type();
                if (type == eBuildingType::temple ||
                    type == eBuildingType::templeTile)
                    continue;
                drawGenericBuildPreviewPart(
                    tp, eb.fB.get(), eb.fBR.get(), t, eb.fTx, eb.fTy,
                    0, dir, canBuildPreview);
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
