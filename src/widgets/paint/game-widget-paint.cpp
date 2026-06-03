#include "widgets/game-widget.h"
#include "widgets/etilepainter.h"

#include <functional>

#include "enumbers.h"

#include "characters/actions/walkable/walkable-object.h"

#include "widgets/eterraineditmenu.h"

#include "textures/etiletotexture.h"
#include "textures/egametextures.h"

#include "textures/eparktexture.h"
#include "textures/evaryingsizetex.h"

#include "buildings/allbuildings.h"
#include "widgets/paint/sanctuary-preview.h"
#include "widgets/paint/sanctuary-real-draw.h"
#include "buildings/eheatgetters.h"
#include "buildings/pyramids/epyramid.h"

#include "missiles/emissile.h"
#include "characters/soldier-banner.h"
#include "widgets/paint/invasion-debug-paint.h"
#include "widgets/paint/draw/draw-column.h"
#include "widgets/paint/draw/dont-draw-appeal.h"

#include "spawners/elandinvasionpoint.h"

#include <algorithm>

#include "characters/esoldier.h"
#include "characters/actions/soldier-action.h"
#include "characters/actions/cart-transporter-action.h"

#include "evectorhelpers.h"
#include "etilehelper.h"
#include "emainwindow.h"
#include "widgets/eminimap.h"
#include "widgets/gamebuild/ecommonhousingbuild.h"

#include "eiteratesquare.h"
#include "widgets/efonts.h"

#include <array>
#include <cmath>
#include <map>
#include <set>
#include <string>

class GameBoardRegisterLock
{
public:
    GameBoardRegisterLock(GameBoard &board) : mBoard(board)
    {
        mBoard.setRegisterBuildingsEnabled(false);
    }
    ~GameBoardRegisterLock()
    {
        mBoard.setRegisterBuildingsEnabled(true);
    }

private:
    GameBoard &mBoard;
};


void GameWidget::paintEvent(ePainter &p)
{
    auto* r = p.renderer();
    const int w = width();
    const int h = height();
    if (!mWorldTex || mWorldTex->width() != w || mWorldTex->height() != h) {
        mWorldTex = std::make_shared<eTexture>();
        mWorldTex->create(r, w, h);
        SDL_SetTextureScaleMode(mWorldTex->tex(), SDL_ScaleModeNearest);
    }
    mWorldTex->setAsRenderTarget(r);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
    SDL_RenderClear(r);

    if (mUpdateViewedTileScheduled)
    {
        mUpdateViewedTileScheduled = false;
        const auto oldC = mViewedTile ? mViewedTile->cityId() : eCityId::neutralFriendly;
        mViewedTile = viewedTile();
        const auto newC = mViewedTile ? mViewedTile->cityId() : eCityId::neutralFriendly;
        mViewedCityId = newC;
        if (oldC != newC)
        {
            mBoard->clearBannerSelection();
            mBoard->clearTriremeSelection();
            const auto c = mBoard->boardCityWithId(newC);
            if (!mEditorMode && c && c->owningPlayer() == nullptr)
            {
                showBuyCity(newC);
            }
            else
            {
                hideBuyCity();
            }
            mGm->viewedCityChanged();
        }
    }
    if (mBoard->duringEarthquake())
    {
        mDX += (eRand::rand() % 11) - 5;
        mDY += (eRand::rand() % 11) - 5;
        clampViewBox();
    }
    {
        const auto &ss = mBoard->selectedSoldiers();
        const auto &selectedTriremes = mBoard->selectedTriremes();
        const bool v = !ss.empty() || !selectedTriremes.empty();
        setArmyMenuVisible(v);

        bool home = false;
        if (ss.empty())
        {
            home = false;
        }
        else
        {
            for (const auto &s : ss)
            {
                const bool h = s->isHome();
                if (h)
                {
                    home = true;
                    break;
                }
            }
        }
        mAm->setSoldiersHome(home);
    }
    {
        const auto pbv = eViewMode::patrolBuilding;
        if (!mPatrolBuilding && (mViewMode == pbv || mPatrolPathWid))
        {
            setPatrolBuilding(nullptr);
        }
    }
    const auto nowTp = std::chrono::steady_clock::now();
    double dtMs = 0.0;
    if(mLastPaintTpValid) {
        using ms_t = std::chrono::duration<double, std::milli>;
        dtMs = std::chrono::duration_cast<ms_t>(nowTp - mLastPaintTp).count();
        if(dtMs > 250.0) dtMs = 250.0;
    } else {
        mLastPaintTpValid = true;
    }
    mLastPaintTp = nowTp;
    if(dtMs <= 0.0) dtMs = kBaseRenderMs;
    mLastDtMs = dtMs;
    const double dtScale = dtMs / kBaseRenderMs;

    mFrame++;
    const int prevAnimFrame = mAnimFrame;
    int simTicks = 0;
    mSimAccumMs += dtMs;
    while(mSimAccumMs >= kSimStepMs) {
        mSimAccumMs -= kSimStepMs;
        simTicks++;
    }
    mAnimAccumMs += dtMs;
    while(mAnimAccumMs >= kAnimStepMs) {
        mAnimAccumMs -= kAnimStepMs;
        mAnimFrame++;
    }
    mRotateAccumMs += dtMs;
    while(mRotateAccumMs >= kBaseRenderMs) {
        mRotateAccumMs -= kBaseRenderMs;
        mRotateFrame++;
    }
    bool updateTips = false;
    for (int i = 0; i < int(mTips.size()); i++)
    {
        const auto &tip = mTips[i];
        if (mBoard->totalTime() > tip.fLastTick)
        {
            tip.fWid->deleteLater();
            mTips.erase(mTips.begin() + i);
            updateTips = true;
            i--;
        }
    }
    if (updateTips)
        updateTipPositions();

    // Update toasts - remove expired ones
    bool updateToasts = false;
    for (int i = 0; i < int(mToasts.size()); i++)
    {
        const auto &toast = mToasts[i];
        if (mBoard->totalTime() > toast.fExpireTick)
        {
            toast.fWid->deleteLater();
            mToasts.erase(mToasts.begin() + i);
            updateToasts = true;
            i--;
        }
    }
    // Promote pending toasts; in turbo promote all at once with no cap
    {
        const bool turbo = mSpeedId == sMaxSpeedId;
        while (!mPendingToasts.empty() && (turbo || mToasts.size() < 3))
        {
            eToast toast = mPendingToasts.front();
            mPendingToasts.pop_front();
            toast.fExpireTick = mBoard->totalTime() + 14 * eNumbers::sDayLength;
            createToastWidget(toast);
            mToasts.push_back(toast);
            updateToasts = true;
        }
    }
    if (updateToasts)
        updateToastPositions();
    if (mSpeedLabel && mFrame > mSpeedLabelHideFrame)
    {
        mSpeedLabel->deleteLater();
        mSpeedLabel = nullptr;
    }
    if (mZoomLabel && mFrame > mZoomLabelHideFrame)
    {
        mZoomLabel->deleteLater();
        mZoomLabel = nullptr;
        updateTipPositions();
    }
    if (mAnimFrame != prevAnimFrame)
        mBoard->incFrame();

    const bool turbo = mSpeedId == sMaxSpeedId;
    const int iMax = turbo ? 4 : simTicks;
    for (int i = 0; i < iMax; i++)
    {
        mBoard->scheduleDataUpdate();
        mBoard->updateAppealMapIfNeeded();
        mBoard->handleFinishedTasks();
        const bool incTime = !mPaused && !mLocked && !mMsgBox && !hasInfoWidget();
        if (incTime)
        {
            const bool lost = mBoard->episodeLost();
            if (lost)
            {
                const auto w = window();
                w->episodeLost();
            }
            else
            {
                mTime += mSpeed;
                int remaining = mSpeed;
                while(remaining > 0) {
                    const int step = std::min(remaining, sSpeeds[2]);
                    mBoard->incTime(step);
                    remaining -= step;
                }
                mGm->update();
            }
        }
        mBoard->emptyRubbish();
        if (!incTime)
            break;
    }

    if (!window()->settings().fDisableEdgeScroll)
    {
        const int edgeStep = std::max(1, int(std::lround(35.0 * dtScale)));
        if (mHoverX == 0)
        {
            setDX(mDX + edgeStep);
        }
        else if (mHoverX == width() - 1)
        {
            setDX(mDX - edgeStep);
        }
        if (mHoverY == 0)
        {
            setDY(mDY + edgeStep);
        }
        else if (mHoverY == height() - 1)
        {
            setDY(mDY - edgeStep);
        }
    }
    smoothScroll();
    GameBoardRegisterLock lock(*mBoard);

    p.setFont(eFonts::defaultFont(resolution()));
    p.translate(mDX, mDY);
    eTilePainter tp(p, mTileSize, mTileW, mTileH);
    const auto &numbers = mNumbers[mTileSize];
    std::vector<std::pair<int, int>> trackingBoxes;
    // enemy banners scheduled after all tiles so they sit on top of everything
    std::vector<std::function<void()>> deferredEnemyBanners;
    std::vector<std::pair<int, int>> cartProblemBoxes;

    const auto ppid = mBoard->personPlayer();

    const int tid = static_cast<int>(mTileSize);
    const auto &trrTexs = eGameTextures::terrain().at(tid);
    const auto &builTexs = eGameTextures::buildings().at(tid);
    const auto &destTexs = eGameTextures::destrution().at(tid);
    const auto &charTexs = eGameTextures::characters().at(tid);
    const auto dir = mBoard->direction();
    const int boardWidth = mBoard->width();
    const int boardHeight = mBoard->height();

    const auto mode = mGm->mode();

    const int sMinX = std::min(mPressedTX, mHoverTX);
    const int sMinY = std::min(mPressedTY, mHoverTY);
    const int sMaxX = std::max(mPressedTX, mHoverTX);
    const int sMaxY = std::max(mPressedTY, mHoverTY);

    const bool terrUpdated = mBoard->terrainUpdateScheduled();
    if (terrUpdated)
    {
        updateTerrainTextures();
        mBoard->afterTerrainUpdated();
    }

    const bool terrainEditing = mTem->visible();
    const bool fogOfWar = !terrainEditing && mBoard->fogOfWar();
    std::set<eMonument*> drawnTempleWoman;
    std::vector<SanctuaryTempleDebugDot> templeDebugDots;
    const auto drawTerrain = [&](eTile *const tile)
    {
        const int worldTileX = tile->x();
        const int worldTileY = tile->y();

        const auto terr = tile->terrain();

        auto border = tile->territoryBorder();
        int viewTileX;
        int viewTileY;
        eTileHelper::tileIdToRotatedTileId(worldTileX, worldTileY,
                                           viewTileX, viewTileY, dir,
                                           boardWidth, boardHeight);

        const int ta = tile->altitude();

        switch (dir)
        {
        case eWorldDirection::N:
        {
        }
        break;
        case eWorldDirection::E:
        {
            border.fT = border.fR;
            border.fTL = border.fTR;
            border.fTR = border.fBR;
        }
        break;
        case eWorldDirection::S:
        {
            border.fT = border.fB;
            border.fTR = border.fBL;
            border.fTL = border.fBR;
        }
        break;
        case eWorldDirection::W:
        {
            border.fT = border.fL;
            border.fTR = border.fTL;
            border.fTL = border.fBL;
        }
        break;
        }

        const auto cid = tile->cityId();
        const bool tileFogOfWar = fogOfWar &&
                                  cid == eCityId::neutralFriendly;

        if (tile->updateTerrain() || tile->hasRoad())
        {
            if (!terrUpdated)
            {
                updateTerrainTextures(tile, trrTexs, builTexs);
            }
            tile->terrainUpdated();
        }
        const auto &painter = tile->terrainPainter();

        const int drawDim = painter.fDrawDim;

        double drawX;
        double drawY;
        const int a = mDrawElevation ? tile->altitude() : 0;
        drawXY(worldTileX, worldTileY, drawX, drawY, drawDim, drawDim, a);

        stdsptr<eTexture> tex;
        if (drawDim == 0)
        {
            const auto u = tile->underTile();
            if (u)
            {
                const auto &upainter = u->terrainPainter();
                const int dx = tile->underTileDX();
                const int dy = tile->underTileDY();
                const int uDrawDim = upainter.fDrawDim;
                const auto type = u->underBuildingType();
                const bool isPark = type == eBuildingType::park;
                if (isPark && !(dy == uDrawDim - 1 && dx == 0))
                    return;
                const bool textureFitTileX = dx == uDrawDim - 1;
                const bool textureFitTileY = dy == uDrawDim - 1;
                if (textureFitTileX || textureFitTileY)
                {
                    drawX += 0.5 * (uDrawDim - 1) - dx;
                    drawY += 1.5 * (uDrawDim - 1) - dy;
                    tex = upainter.getTexture(mAnimFrame);
                    if (tex && !isPark)
                    {
                        SDL_Rect clipRect;
                        clipRect.y = -10000;
                        clipRect.h = 20000;
                        const int d = textureFitTileY ? 1 : 0;
                        clipRect.x = mDX + (viewTileX - viewTileY - d) * mTileW / 2;
                        clipRect.w = textureFitTileX && textureFitTileY ? mTileW : mTileW / 2;
                        SDL_RenderSetClipRect(p.renderer(), &clipRect);
                    }
                }
            }
        }
        else if (drawDim == 1)
        {
            tex = painter.getTexture(mAnimFrame);
        }

        if (tex)
        {
            bool lavaCm = false;
            bool eraseCm = false;
            bool repairCm = false;
            bool patrolCm = false;
            bool editorHover = false;
            if (terrainEditing)
            {
                editorHover = eVectorHelpers::contains(mHoverTiles, tile) ||
                              eVectorHelpers::contains(mInflTiles, tile);
                if (editorHover)
                {
                    tex->setColorMod(255, 175, 175);
                }
            }
            const bool lavaFlows = mBoard->duringLavaFlow();
            const bool hasLava = terr == eTerrain::lava;
            if (lavaFlows && hasLava)
            {
                lavaCm = true;
                tex->setColorMod(255, 0, 0);
            }
            else if (mDestinationBuilding && !mDestinationPath.empty() &&
                     eVectorHelpers::contains(mDestinationPath, tile))
            {
                patrolCm = true;
                tex->setColorMod(175, 255, 175);
            }
            else if (mPatrolBuilding &&
                     (!mPatrolPath.empty() || !mPatrolPath1.empty()))
            {
                const bool bothDirections = mPatrolBuilding->bothDirections();
                patrolCm = eVectorHelpers::contains(mPatrolPath, tile) ||
                           (bothDirections &&
                            eVectorHelpers::contains(mPatrolPath1, tile));
                if (patrolCm)
                {
                    tex->setColorMod(175, 255, 175);
                }
                else
                {
                    patrolCm = eVectorHelpers::contains(mExcessPatrolPath, tile) ||
                               (bothDirections &&
                                eVectorHelpers::contains(mExcessPatrolPath1, tile));
                    if (patrolCm)
                    {
                        tex->setColorMod(255, 175, 175);
                    }
                }
            }
            else if ((!terrainEditing &&
                      !static_cast<bool>(terr & eTerrain::stones)) ||
                     (terrainEditing &&
                      mTem->brushType() == eBrushType::apply))
            {
                const auto building = tile->underBuilding();
                if (building)
                {
                    eraseCm = inErase(building);
                }
                else
                {
                    eraseCm = inErase(worldTileX, worldTileY);
                }
                if (eraseCm)
                    tex->setColorMod(255, 175, 175);
            }

            if (mEditorMode && !eraseCm && !patrolCm)
            {
                const auto building = tile->underBuilding();
                if (building)
                {
                    const int eid = building->districtId();
                    const auto ecid = mBoard->currentDistrictId();
                    if (ecid == eid)
                    {
                        tex->setColorMod(200, 255, 200);
                    }
                    else
                    {
                        tex->setColorMod(255, 200, 200);
                    }
                }
                else
                {
                    if (tile->tidalWaveZone())
                    {
                        tex->setColorMod(0, 0, 255);
                    }
                    else if (tile->lavaZone())
                    {
                        tex->setColorMod(255, 0, 0);
                    }
                    else if (tile->landSlideZone())
                    {
                        tex->setColorMod(255, 0, 255);
                    }
                }
            }

            if (mode == eBuildingMode::repair)
            {
                repairCm = inRepair(worldTileX, worldTileY);
                if (repairCm)
                    tex->setColorMod(175, 175, 255);
            }

            bool defaultHover = false;
            if (mode == eBuildingMode::none && !terrainEditing)
            {
                defaultHover = worldTileX == mHoverTX && worldTileY == mHoverTY;
                const auto building = tile->underBuilding();
                if (building && building->type() == eBuildingType::park)
                {
                    const auto parent = eraseParkParentTileAt(mHoverTX, mHoverTY);
                    const auto center = building->centerTile();
                    defaultHover = parent && center &&
                                   (center == parent ||
                                    center->underTile() == parent);
                }
                if (defaultHover)
                    tex->setColorMod(200, 200, 200);
            }

            if (tileFogOfWar)
            {
                const int maxDist = eTile::sMaxDistanceToBorder;
                const double dist = tile->distanceToBorder();
                const int val = std::round((maxDist - dist) * 255 / maxDist);
                tex->setColorMod(val, val, val);
            }
            if (hasLava)
            {
                const int seed = tile->seed();
                const auto &coll = trrTexs.fDryTerrainTexs;
                const int texId = seed % coll.size();
                const auto tex = coll.getTexture(texId);
                tp.drawTexture(drawX, drawY, tex, eAlignment::top);
            }
            tp.drawTexture(drawX, drawY, tex, eAlignment::top);
            if (drawDim == 0)
                SDL_RenderSetClipRect(p.renderer(), nullptr);
            if (eraseCm || repairCm || patrolCm || editorHover ||
                mEditorMode || tileFogOfWar || lavaCm || defaultHover)
            {
                tex->clearColorMod();
            }
        }

        {
            if (!tileFogOfWar || true)
            {
                const int dim = mTileW / 10;
                const SDL_Color color{255, 255, 255, 255};
                if (border.fT)
                {
                    tp.fillRectCenter(viewTileX - ta, viewTileY - ta, dim, dim, color);
                }
                if (border.fTR)
                {
                    tp.fillRectCenter(viewTileX + 0.5 - ta, viewTileY - ta, dim, dim, color);
                }
                if (border.fTL)
                {
                    tp.fillRectCenter(viewTileX - ta, viewTileY + 0.5 - ta, dim, dim, color);
                }
            }
        }
    };

    std::vector<eTile *> bridgetTs;
    bool bridgeValid = false;
    bool bridgeRot;
    if (mode == eBuildingMode::bridge)
    {
        const auto startTile = mBoard->tile(mHoverTX, mHoverTY);
        bridgeValid = bridgeTiles(startTile, eTerrain::water, bridgetTs, bridgeRot);
        if (!bridgeValid)
            bridgeValid = bridgeTiles(startTile, eTerrain::quake, bridgetTs, bridgeRot);
    }

    constexpr SDL_Color selectedWalkerColor{24, 255, 24, 255};
    const auto &selectedPatrolerColor = selectedWalkerColor;

    eRoadPreviewPath patrolRoadPreview;
    eTile *patrolRoadStart = nullptr;
    eTile *patrolRoadReturn = nullptr;
    if (mPatrolBuilding)
    {
        if (!mPatrolBuilding->patrolGuides().empty())
        {
            addPathBands(mExcessPatrolPath, patrolRoadPreview);
            addPathBands(mPatrolPath, patrolRoadPreview);
            patrolRoadStart = firstPathRoad(mExcessPatrolPath);
            if (!patrolRoadStart)
                patrolRoadStart = firstPathRoad(mPatrolPath);
            patrolRoadReturn = lastPathRoad(mPatrolPath);
            if (!patrolRoadReturn)
                patrolRoadReturn = lastPathRoad(mExcessPatrolPath);
            if (mPatrolBuilding->bothDirections())
            {
                addPathBands(mExcessPatrolPath1, patrolRoadPreview);
                addPathBands(mPatrolPath1, patrolRoadPreview);
                if (!patrolRoadReturn)
                {
                    patrolRoadReturn = lastPathRoad(mPatrolPath1);
                    if (!patrolRoadReturn)
                    {
                        patrolRoadReturn = lastPathRoad(mExcessPatrolPath1);
                    }
                }
            }
        }
        else
        {
            const auto buildingType = mPatrolBuilding->type();
            const bool agora = buildingType == eBuildingType::commonAgora ||
                               buildingType == eBuildingType::grandAgora;
            if (agora)
            {
                const auto ab = static_cast<eAgoraBase*>(mPatrolBuilding.get());
                const auto start = ab->agoraRoadStart();
                const auto ret = ab->agoraRoadEnd();
                if (start)
                {
                    patrolRoadStart = start;
                    patrolRoadReturn = ret;
                    const auto walkable = WalkableObject::sCreateRoadblock();
                    addRoamerPreview(start, patrolRoadPreview, walkable);
                }
            }
            else
            {
                const auto roads = mPatrolBuilding->surroundingRoad(false, true);
                if (!roads.empty())
                {
                    patrolRoadStart = roads.front();
                    patrolRoadReturn = roads.back();
                    const auto walkable = WalkableObject::sCreateRoadblock();
                    addRoamerPreview(patrolRoadStart, patrolRoadPreview, walkable);
                    if (mPatrolBuilding->bothDirections() &&
                        patrolRoadReturn != patrolRoadStart)
                    {
                        addRoamerPreview(patrolRoadReturn, patrolRoadPreview, walkable);
                    }
                }
            }
        }
    }
    const auto drawSelectedRoadPreview = [&](eTile *const tile)
    {
        if (!mPatrolBuilding || !tile)
            return;
        drawRoadBandTile(tile, patrolRoadStart, patrolRoadReturn,
                         patrolRoadPreview, tp, trrTexs);
    };

    const auto buildingDrawer = [&](eTile *const tile)
    {
        const int worldTileX = tile->x();
        const int worldTileY = tile->y();
        int viewTileX;
        int viewTileY;
        eTileHelper::tileIdToRotatedTileId(worldTileX, worldTileY,
                                           viewTileX, viewTileY, dir,
                                           boardWidth, boardHeight);
        const int a = mDrawElevation ? tile->altitude() : 0;
        const int da = mDrawElevation ? tile->doubleAltitude() : 0;

        auto building = tile->underBuilding();
        if (building && building->type() == eBuildingType::road)
        {
            const auto r = static_cast<eRoad *>(building);
            if (const auto h = r->aboveHippodrome())
            {
                building = h;
            }
        }
        const auto buildingType = building ? building->type() : eBuildingType::none;

        const bool bv = eViewModeHelpers::buildingVisible(mViewMode, building);
        const bool v = building && bv;
        const bool drawsCharactersInsteadOfTexture =
            eBuilding::sFlatBuilding(buildingType);

        double drawX;
        double drawY;
        drawXY(worldTileX, worldTileY, drawX, drawY, 1, 1, a);

        const auto drawBlessedCursed = [&](const double bx, const double by)
        {
            if (building->blessed())
            {
                eGameTextures::loadBlessed();
                const auto &blsd = destTexs.fBlessed;
                const auto tex = blsd.getTexture(building->textureTime() % blsd.size());
                tp.drawTexture(bx, by, tex, eAlignment::bottom);
            }
            else if (building->cursed())
            {
                eGameTextures::loadCursed();
                const auto &blsd = destTexs.fCursed;
                const auto tex = blsd.getTexture(building->textureTime() % blsd.size());
                tp.drawTexture(bx, by, tex, eAlignment::bottom);
            }
        };

        const auto drawFire = [&](eTile *const ubt)
        {
            const int worldTileX = ubt->x();
            const int worldTileY = ubt->y();
            double frx;
            double fry;
            drawXY(worldTileX, worldTileY, frx, fry, 1, 1, a);
            eGameTextures::loadFire();
            const int f = (worldTileX + worldTileY) % destTexs.fFire.size();
            const auto &ff = destTexs.fFire[f];
            const int dt = mBoard->frame() + std::abs(worldTileX * worldTileY);
            const auto tex = ff.getTexture(dt % ff.size());
            tp.drawTexture(frx + 1, fry, tex, eAlignment::hcenter | eAlignment::top);
        };

        const auto drawBuildingModes = [&]()
        {
            if (!building)
                return;
            const double cdx = -0.65;
            const double cdy = -0.65;
            if (mViewMode == eViewMode::hazards)
            {
                const auto pid = mBoard->personPlayer();
                const auto diff = mBoard->difficulty(pid);
                const int fr = DifficultyHelpers::fireRisk(diff, buildingType);
                const int dr = DifficultyHelpers::damageRisk(diff, buildingType);
                if (const auto h = dynamic_cast<eHouseBase *>(building))
                {
                    if (h->people() == 0)
                        return;
                }
                const int h = 100 - building->maintenance();
                if ((fr || dr) && h > 5)
                {
                    const int n = h / 15;
                    const eTextureCollection *coll = nullptr;
                    if (n < 2)
                    {
                        coll = &builTexs.fColumn1;
                    }
                    else if (n < 3)
                    {
                        coll = &builTexs.fColumn2;
                    }
                    else if (n < 4)
                    {
                        coll = &builTexs.fColumn3;
                    }
                    else
                    {
                        coll = &builTexs.fColumn4;
                    }

                    drawColumn(tp, n, drawX + cdx, drawY + cdy, *coll);
                }
            }
            else if (mViewMode == eViewMode::taxes)
            {
                if (const auto h = dynamic_cast<eHouseBase *>(building))
                {
                    if (h->people() == 0)
                        return;
                    const bool paid = h->paidTaxes();
                    const int n = paid ? 4 : 0;
                    drawColumn(tp, n, drawX + cdx, drawY + cdy, builTexs.fColumn1);
                }
            }
            else if (mViewMode == eViewMode::water)
            {
                if (buildingType == eBuildingType::commonHouse)
                {
                    const auto ch = static_cast<SmallHouse *>(building);
                    if (ch->people() == 0)
                        return;
                    const int w = ch->water() / 2;
                    drawColumn(tp, w, drawX + cdx, drawY + cdy, builTexs.fColumn5);
                }
            }
            else if (mViewMode == eViewMode::hygiene)
            {
                if (buildingType == eBuildingType::commonHouse)
                {
                    const auto ch = static_cast<SmallHouse *>(building);
                    if (ch->people() == 0)
                        return;
                    const int h = ch->hygiene();
                    const int n = h / 15;
                    const eTextureCollection *coll = nullptr;
                    if (n < 2)
                    {
                        coll = &builTexs.fColumn4;
                    }
                    else if (n < 3)
                    {
                        coll = &builTexs.fColumn3;
                    }
                    else if (n < 4)
                    {
                        coll = &builTexs.fColumn2;
                    }
                    else
                    {
                        coll = &builTexs.fColumn1;
                    }

                    drawColumn(tp, n, drawX + cdx, drawY + cdy, *coll);
                }
            }
            else if (mViewMode == eViewMode::unrest)
            {
                if (buildingType == eBuildingType::commonHouse)
                {
                    const auto ch = static_cast<SmallHouse *>(building);
                    if (ch->people() == 0)
                        return;
                    const int h = 100 - ch->satisfaction();
                    const int n = h / 15;
                    const eTextureCollection *coll = nullptr;
                    if (n < 2)
                    {
                        coll = &builTexs.fColumn1;
                    }
                    else if (n < 3)
                    {
                        coll = &builTexs.fColumn2;
                    }
                    else if (n < 4)
                    {
                        coll = &builTexs.fColumn3;
                    }
                    else
                    {
                        coll = &builTexs.fColumn4;
                    }

                    drawColumn(tp, n, drawX + cdx, drawY + cdy, *coll);
                }
            }
            else if (mViewMode == eViewMode::actors ||
                     mViewMode == eViewMode::astronomers)
            {
                if (buildingType == eBuildingType::commonHouse ||
                    buildingType == eBuildingType::eliteHousing)
                {
                    const auto ch = static_cast<eHouseBase *>(building);
                    if (ch->people() == 0)
                        return;
                    const int a = ch->actorsAstronomers() > 0 ? 1 : 0;
                    if(a > 0) drawColumn(tp, a, drawX + cdx, drawY + cdy, builTexs.fColumn1);
                }
            }
            else if (mViewMode == eViewMode::philosophers ||
                     mViewMode == eViewMode::inventors)
            {
                if (buildingType == eBuildingType::commonHouse ||
                    buildingType == eBuildingType::eliteHousing)
                {
                    const auto ch = static_cast<eHouseBase *>(building);
                    if (ch->people() == 0)
                        return;
                    const int a = ch->philosophersInventors() > 0 ? 1 : 0;
                    if(a > 0) drawColumn(tp, a, drawX + cdx, drawY + cdy, builTexs.fColumn1);
                }
            }
            else if (mViewMode == eViewMode::athletes ||
                     mViewMode == eViewMode::scholars)
            {
                if (buildingType == eBuildingType::commonHouse ||
                    buildingType == eBuildingType::eliteHousing)
                {
                    const auto ch = static_cast<eHouseBase *>(building);
                    if (ch->people() == 0)
                        return;
                    const int a = ch->athletesScholars() > 0 ? 1 : 0;
                    if(a > 0) drawColumn(tp, a, drawX + cdx, drawY + cdy, builTexs.fColumn1);
                }
            }
            else if (mViewMode == eViewMode::competitors ||
                     mViewMode == eViewMode::curators)
            {
                if (buildingType == eBuildingType::commonHouse ||
                    buildingType == eBuildingType::eliteHousing)
                {
                    const auto ch = static_cast<eHouseBase *>(building);
                    if (ch->people() == 0)
                        return;
                    const int a = ch->competitorsCurators() > 0 ? 1 : 0;
                    if(a > 0) drawColumn(tp, a, drawX + cdx, drawY + cdy, builTexs.fColumn1);
                }
            }
            else if (mViewMode == eViewMode::allCulture ||
                     mViewMode == eViewMode::allScience)
            {
                if (buildingType == eBuildingType::commonHouse ||
                    buildingType == eBuildingType::eliteHousing)
                {
                    const auto ch = static_cast<eHouseBase *>(building);
                    if (ch->people() == 0)
                        return;
                    int a = 0;
                    if(ch->philosophersInventors() > 0) a++;
                    if(ch->actorsAstronomers() > 0) a++;
                    if(ch->athletesScholars() > 0) a++;
                    if(ch->competitorsCurators() > 0) a++;
                    const auto& bd = ch->getBoard();
                    const auto bc = bd.boardCityWithId(ch->cityId());
                    const bool atl = ch->atlantean();
                    if(bc && (atl ? bc->museumBonusActive() : bc->stadiumBonusActive())) a++;
                    if(a > 0) drawColumn(tp, a, drawX + cdx, drawY + cdy, builTexs.fColumn1);
                }
            }
            else if (mViewMode == eViewMode::supplies)
            {
                if (buildingType == eBuildingType::commonHouse)
                {
                    const auto ch = static_cast<SmallHouse *>(building);
                    if (ch->people() == 0)
                        return;
                    double rxx = drawX - 2.5;
                    double ryy = drawY - 2;
                    tp.scheduleDrawTexture(rxx, ryy, builTexs.fSuppliesBg);
                    rxx += 0.49;
                    ryy += 0.15;
                    const double inc = 0.43;
                    tp.scheduleDrawTexture(rxx, ryy,
                                           ch->lowFood() ? builTexs.fNHasFood : builTexs.fHasFood);
                    rxx += inc;
                    ryy -= inc;
                    tp.scheduleDrawTexture(rxx, ryy,
                                           ch->lowFleece() ? builTexs.fNHasFleece : builTexs.fHasFleece);
                    rxx += inc;
                    ryy -= inc;
                    tp.scheduleDrawTexture(rxx, ryy,
                                           ch->lowOil() ? builTexs.fNHasOil : builTexs.fHasOil);
                }
                else if (buildingType == eBuildingType::eliteHousing)
                {
                    const auto ch = static_cast<EliteHousing *>(building);
                    if (ch->people() == 0)
                        return;
                    double rxx = drawX - 3.5;
                    double ryy = drawY - 1.5;
                    tp.scheduleDrawTexture(rxx, ryy, builTexs.fEliteSuppliesBg);
                    rxx += 0.49;
                    ryy += 0.15;
                    const double inc = 0.43;
                    tp.scheduleDrawTexture(rxx, ryy,
                                           ch->lowFood() ? builTexs.fNHasFood : builTexs.fHasFood);
                    rxx += inc;
                    ryy -= inc;
                    tp.scheduleDrawTexture(rxx, ryy,
                                           ch->lowFleece() ? builTexs.fNHasFleece : builTexs.fHasFleece);
                    rxx += inc;
                    ryy -= inc;
                    tp.scheduleDrawTexture(rxx, ryy,
                                           ch->lowOil() ? builTexs.fNHasOil : builTexs.fHasOil);
                    rxx += inc;
                    ryy -= inc;
                    tp.scheduleDrawTexture(rxx, ryy,
                                           ch->lowWine() ? builTexs.fNHasWine : builTexs.fHasWine);
                    rxx += inc;
                    ryy -= inc;
                    tp.scheduleDrawTexture(rxx, ryy,
                                           ch->lowArms() ? builTexs.fNHasArms : builTexs.fHasArms);
                    rxx += inc;
                    ryy -= inc;
                    tp.scheduleDrawTexture(rxx, ryy,
                                           ch->lowHorses() ? builTexs.fNHasHorses : builTexs.fHasHorses);
                }
            }
        };

        if (building && !v)
        {
            if (mViewMode != eViewMode::appeal)
            {
                const auto tex = getBasementTexture(viewTileX, viewTileY, building, trrTexs,
                                                    dir, boardWidth, boardHeight);
                tp.drawTexture(drawX, drawY, tex, eAlignment::top);
            }
        }
        else if (building && !drawsCharactersInsteadOfTexture)
        {
            const auto getDisplacement =
                [](const int w, const int h,
                   double &dx, double &dy)
            {
                if (w == 1 && h == 1)
                {
                    dx = -0.5;
                    dy = 0.5;
                }
                else if (w == 2 && h == 2)
                {
                    dx = -1.;
                    dy = 1.;
                }
                else if (w == 3 && h == 3)
                {
                    dx = -1.5;
                    dy = 1.5;
                }
                else if (w == 4 && h == 4)
                {
                    dx = -2.;
                    dy = 2.;
                }
                else if (w == 5 && h == 5)
                {
                    dx = -2.5;
                    dy = 2.5;
                }
                else if (w == 6 && h == 6)
                {
                    dx = -3.0;
                    dy = 3.0;
                }
                else
                {
                    dx = 0.;
                    dy = 0.;
                }
            };
            const auto size = tp.size();
            const auto textureSpace = building->getTextureSpace(worldTileX, worldTileY, size);
            const auto &textureWorldRect = textureSpace.fRect;
            const auto textureViewRect = eTileHelper::toRotatedRect(
                textureWorldRect, dir, boardWidth, boardHeight);
            const int textureFitTileY = textureViewRect.y + textureViewRect.h - 1;
            const int textureFitTileX = textureViewRect.x + textureViewRect.w - 1;
            const bool isTextureFitTileX = viewTileX == textureFitTileX;
            const bool isTextureFitTileY = viewTileY == textureFitTileY;
            double dx;
            double dy;
            getDisplacement(textureWorldRect.w, textureWorldRect.h, dx, dy);
            const double buildingDrawX = textureFitTileX + dx + 1 - da * 0.5;
            const double buildingDrawY = textureFitTileY + dy + 1 - da * 0.5;
            if (isTextureFitTileX || isTextureFitTileY)
            {
                const bool last = isTextureFitTileX && isTextureFitTileY;
                if (textureSpace.fClamp)
                {
                    SDL_Rect clipRect;
                    clipRect.y = -10000;
                    clipRect.h = 20000;
                    const int d = isTextureFitTileY ? 1 : 0;
                    clipRect.x = mDX + (viewTileX - viewTileY - d) * mTileW / 2;
                    clipRect.w = last ? mTileW : mTileW / 2;
                    const int margin = 5 * mTileW;
                    if (viewTileX == textureFitTileX && viewTileY == textureViewRect.y)
                    {
                        if (dir == eWorldDirection::N ||
                            dir == eWorldDirection::S)
                        {
                            clipRect.w += margin;
                        }
                        else
                        {
                            clipRect.x -= margin;
                            clipRect.w += margin;
                        }
                    }
                    if (viewTileY == textureFitTileY && viewTileX == textureViewRect.x)
                    {
                        if (dir == eWorldDirection::N ||
                            dir == eWorldDirection::S)
                        {
                            clipRect.x -= margin;
                            clipRect.w += margin;
                        }
                        else
                        {
                            clipRect.w += margin;
                        }
                    }
                    SDL_RenderSetClipRect(p.renderer(), &clipRect);
                }

                const bool erase = inErase(building);
                const bool hover = inPatrolBuildingHover(building);
                const bool walkerBuildingSelected = mWalkerBuilding && building == mWalkerBuilding.get();
                const bool destSrcSelected = mDestinationBuilding && building == mDestinationBuilding.get();
                const bool destTargetSelected = mDestinationBuilding &&
                    eVectorHelpers::contains(mDestinationTargets, building);
                const bool buildingHovered = [&]()
                {
                    if (mode != eBuildingMode::none)
                        return false;
                    const SDL_Point hp{mHoverTX, mHoverTY};
                    const auto r = building->tileRect();
                    return static_cast<bool>(SDL_PointInRect(&hp, &r));
                }();
                bool colorMod = false;
                int cred = 255;
                int cgreen = 255;
                int cblue = 255;
                if (erase)
                {
                    colorMod = true;
                    cred = 255;
                    cgreen = 175;
                    cblue = 175;
                }
                else if (hover || walkerBuildingSelected || destSrcSelected)
                {
                    colorMod = true;
                    cred = 175;
                    cgreen = 255;
                    cblue = 255;
                }
                else if (destTargetSelected)
                {
                    colorMod = true;
                    cred = 175;
                    cgreen = 255;
                    cblue = 175;
                }
                else if (buildingHovered)
                {
                    colorMod = true;
                    cred = 200;
                    cgreen = 200;
                    cblue = 200;
                }
                else if (mEditorMode)
                {
                    colorMod = true;
                    const int eid = building->districtId();
                    const auto ecid = mBoard->currentDistrictId();
                    if (ecid == eid)
                    {
                        cred = 200;
                        cgreen = 255;
                        cblue = 200;
                    }
                    else
                    {
                        cred = 255;
                        cgreen = 200;
                        cblue = 200;
                    }
                }
                const bool isSancPart =
                    isSanctuaryRealDrawPart(buildingType);
                const auto drawBuildingTexture = [&](const std::shared_ptr<eTexture>& tex) {
                    if(!tex) return;
                    if(colorMod) tex->setColorMod(cred, cgreen, cblue);
                    tp.drawTexture(buildingDrawX + textureSpace.fX, buildingDrawY + textureSpace.fY, tex, eAlignment::top);
                    if(colorMod) tex->clearColorMod();
                };
                const auto drawBuildingOverlays = [&]() {
                    if (!building->overlayEnabled() || !textureSpace.fHasOverlays ||
                        buildingType == eBuildingType::temple ||
                        buildingType == eBuildingType::templeStatue ||
                        buildingType == eBuildingType::templeMonument)
                    {
                        return;
                    }

                    const auto overlays = building->getOverlays(size);
                    for (const auto &o : overlays)
                    {
                        const auto &tex = o.fTex;
                        if (!tex)
                            continue;
                        if (o.fOnTop)
                        {
                            // defer to after the character pass so the pier
                            // dockworker draws over the docked trade boat
                            if (o.fAlignTop)
                            {
                                tp.scheduleDrawTexture(
                                    buildingDrawX + textureSpace.fX + o.fX,
                                    buildingDrawY + textureSpace.fY + o.fY, tex, eAlignment::top);
                            }
                            else
                            {
                                tp.scheduleDrawTexture(
                                    buildingDrawX + textureSpace.fX + o.fX,
                                    buildingDrawY + textureSpace.fY + o.fY, tex);
                            }
                            continue;
                        }
                        if (colorMod)
                            tex->setColorMod(cred, cgreen, cblue);
                        if (o.fAlignTop)
                        {
                            tp.drawTexture(buildingDrawX + textureSpace.fX + o.fX, buildingDrawY + textureSpace.fY + o.fY,
                                           tex, eAlignment::top);
                        }
                        else
                        {
                            tp.drawTexture(buildingDrawX + textureSpace.fX + o.fX, buildingDrawY + textureSpace.fY + o.fY, tex);
                        }
                        if (colorMod)
                            tex->clearColorMod();
                    }
                };
                const auto drawActorsAfterBuildingTexture = [&]() {
                    const auto actorDraws =
                        building->getActorsDrawnAfterBuildingTexture(size);
                    for (const auto& actorDraw : actorDraws)
                    {
                        const auto& tex = actorDraw.fTexture;
                        if(!tex) continue;
                        if(colorMod) tex->setColorMod(cred, cgreen, cblue);
                        if(actorDraw.fUseAlignment)
                        {
                            tp.drawTexture(actorDraw.fViewTileX,
                                           actorDraw.fViewTileY,
                                           tex,
                                           actorDraw.fAlignment);
                        }
                        else
                        {
                            tp.drawTexture(actorDraw.fViewTileX,
                                           actorDraw.fViewTileY,
                                           tex);
                        }
                        if(colorMod) tex->clearColorMod();
                    }
                };
                if (isSancPart && last) {
                    SDL_RenderSetClipRect(p.renderer(), nullptr);
                    const SanctuaryDrawXY drawTileToView = [this](
                        const int tileX,
                        const int tileY,
                        double& drawX,
                        double& drawY,
                        const int tileSpanW,
                        const int tileSpanH,
                        const int altitude)
                    {
                        drawXY(tileX, tileY, drawX, drawY,
                               tileSpanW, tileSpanH, altitude);
                    };
                    drawSanctuaryRealBuildingPart(
                        *mBoard, tp, builTexs,
                        building, buildingType, textureSpace,
                        dir, mAnimFrame, tile,
                        buildingDrawX, buildingDrawY,
                        colorMod, cred, cgreen, cblue,
                        drawTileToView,
                        drawnTempleWoman, templeDebugDots);
                }
                const auto &tex = textureSpace.fTex;
                if (tex && !isSancPart)
                {
                    drawBuildingTexture(tex);
                }
                drawBuildingOverlays();
                SDL_RenderSetClipRect(p.renderer(), nullptr);

                if (last)
                {
                    drawActorsAfterBuildingTexture();
                    bool globalLast = true;
                    if (buildingType == eBuildingType::eliteHousing)
                    {
                        const auto ubRect = building->tileRect();
                        const auto rubRect = eTileHelper::toRotatedRect(
                            ubRect, dir, boardWidth, boardHeight);
                        const int globalFitY = rubRect.y + rubRect.h - 1;
                        const int globalFitX = rubRect.x + rubRect.w - 1;
                        globalLast = viewTileX == globalFitX && viewTileY == globalFitY;
                    }
                    if (buildingType == eBuildingType::commonHouse)
                    {
                        const auto ch = static_cast<SmallHouse *>(building);
                        const bool p = ch->plague();
                        if (p && ch->people())
                        {
                            eGameTextures::loadPlague();
                            const auto &blsd = destTexs.fPlague;
                            const int texId = building->textureTime() % blsd.size();
                            const auto tex = blsd.getTexture(texId);

                            tp.drawTexture(buildingDrawX + 3, buildingDrawY + 1, tex, eAlignment::top);
                        }
                    }
                    if (building->isOnFire())
                    {
                        const auto &ubts = building->tilesUnder();
                        for (const auto &ubt : ubts)
                        {
                            drawFire(ubt);
                        }
                    }
                    if (textureSpace.fHasOverlays && tex)
                    {
                        const int bx = buildingDrawX;
                        const int by = buildingDrawY - textureWorldRect.h;
                        drawBlessedCursed(bx, by);
                    }
                    if (globalLast)
                        drawBuildingModes();
                }
            }
        }
        else if (building)
        {
            if (building->isOnFire())
            {
                const auto &ubts = building->tilesUnder();
                for (const auto &ubt : ubts)
                {
                    drawFire(ubt);
                }
            }
            bool drawBlessed = true;
            if (eResourceBuilding::sIsResourceBuilding(buildingType))
            {
                drawBlessed = worldTileX % 2 && worldTileY % 2;
            }
            if (drawBlessed)
                drawBlessedCursed(drawX + 0.75, drawY);
        }
    };


    iterateOverVisibleTiles([&](eTile *const tile)
                            {
        const int worldTileX = tile->x();
        const int worldTileY = tile->y();
        int viewTileX;
        int viewTileY;
        eTileHelper::tileIdToRotatedTileId(worldTileX, worldTileY,
                                           viewTileX, viewTileY, dir,
                                           boardWidth, boardHeight);
        const int dtx = tile->dx();
        const int dty = tile->dy();
        const int a = mDrawElevation ? tile->altitude() : 0;

        const auto mode = mGm->mode();

        const auto building = tile->underBuilding();
        const auto buildingType = tile->underBuildingType();

        const bool bv = eViewModeHelpers::buildingVisible(mViewMode, building);
        const bool v = building && bv;

        bool bd = false;

        double drawX;
        double drawY;
        drawXY(worldTileX, worldTileY, drawX, drawY, 1, 1, a);

        const auto drawSheepGoat = [&]() {
            if(mode == eBuildingMode::sheep ||
               mode == eBuildingMode::goat ||
               mode == eBuildingMode::cattle ||
               mode == eBuildingMode::erase) {
                if(buildingType == eBuildingType::sheep ||
                   buildingType == eBuildingType::goat ||
                   buildingType == eBuildingType::cattle) {
                    const auto tex = trrTexs.fBuildingBase;
                    const bool e = inErase(building);
                    if(e) tex->setColorMod(255, 175, 175);
                    tp.drawTexture(drawX, drawY, tex, eAlignment::top);
                    if(e) tex->clearColorMod();
                    bd = true;
                }
            }
        };

        const auto drawPatrol = [&]() {
            if(mViewMode == eViewMode::patrolBuilding) {
                if(!building || !mPatrolBuilding) return;
                const auto patrolBuildingType = building->type();
                const bool drawsCharactersInsteadOfTexture =
                    eBuilding::sFlatBuilding(patrolBuildingType);
                if(drawsCharactersInsteadOfTexture) return;
                const auto tex = getBasementTexture(viewTileX, viewTileY, building, trrTexs,
                                                    dir, boardWidth, boardHeight);
                tp.drawTexture(drawX, drawY, tex, eAlignment::top);
                bd = true;
            }
        };

        const auto drawAppeal = [&]() {
            const auto terr = tile->terrain();
            if(dontDrawAppeal(terr)) return;
            if(tile->isElevationTile()) return;
            if(!v && mViewMode == eViewMode::appeal) {
                const auto& am = mBoard->appealMap();
                const auto ae = am.enabled(dtx, dty);
                const bool ch = buildingType == eBuildingType::commonHouse ||
                                buildingType == eBuildingType::eliteHousing;
                if(ae || ch || building) {
                    const bool pyramid = eBuilding::sPyramidBuilding(buildingType);
                    int da = 0;
                    if(pyramid) {
                        const auto p = static_cast<ePyramidElement*>(building);
                        da = 2*p->currentElevation();
                    }
                    const eTextureCollection* coll;
                    if(ch) {
                        coll = &trrTexs.fHouseAppeal;
                    } else {
                        coll = &trrTexs.fAppeal;
                    }
                    const double app = am.heat(dtx, dty);
                    const double mult = app > 0 ? 1 : -1;
                    const double appS = mult*pow(abs(app), 0.75);
                    int appId = (int)std::round(appS + 2.);
                    appId = std::clamp(appId, 0, 9);
                    const auto tex = coll->getTexture(appId);
                    tp.drawTexture(drawX + da, drawY + da, tex, eAlignment::top);
                    bd = true;
                }
            }
        };

        const auto drawCharacters = [&](eTile* const tile,
                                        const bool big,
                                        const bool crosswalk) {
            if(!tile) return;
            const int worldTileX = tile->x();
            const int worldTileY = tile->y();
            int viewTileX;
            int viewTileY;
            eTileHelper::tileIdToRotatedTileId(worldTileX, worldTileY,
                                               viewTileX, viewTileY, dir,
                                               boardWidth, boardHeight);
            const int da = tile->characterDoubleAltitude();
            const auto tileBuildingType = tile->underBuildingType();
            const bool drawsCharactersInsteadOfTexture =
                eBuilding::sFlatBuilding(tileBuildingType);
            const bool hover = worldTileX == mHoverTX && worldTileY == mHoverTY;
            const int hr = 200;
            const int hg = 200;
            const int hb = 255;
            const bool pyramid = eBuilding::sPyramidBuilding(tileBuildingType);
            if(drawsCharactersInsteadOfTexture ||
               tileBuildingType == eBuildingType::wall || pyramid) {
                if(crosswalk) {
                    if(tileBuildingType != eBuildingType::road) return;
                    const auto b = tile->underBuilding();
                    const auto r = static_cast<eRoad*>(b);
                    const auto h = r->aboveHippodrome();
                    if(!h) return;
                } else {
                    if(tileBuildingType == eBuildingType::road) {
                        const auto b = tile->underBuilding();
                        const auto r = static_cast<eRoad*>(b);
                        const auto h = r->aboveHippodrome();
                        if(h) return;
                    }
                }
                const auto r = p.renderer();
                const auto& chars = tile->characters();
                for(const auto& c : chars) {
                    if(!c->visible()) continue;
                    const auto ct = c->type();
                    if(ct == eCharacterType::cartTransporter ||
                       ct == eCharacterType::ox ||
                       ct == eCharacterType::trailer) {
                        if(eBuilding::sSanctuaryBuilding(tileBuildingType)) {
                            continue;
                        }
                    }
                    const bool cbig = ct == eCharacterType::scylla ||
                                      ct == eCharacterType::kraken ||
                                      ct == eCharacterType::enemyBoat ||
                                      ct == eCharacterType::trireme ||
                                      ct == eCharacterType::tradeBoat;
                    if(big != cbig) continue;
                    const bool v = eViewModeHelpers::characterVisible(
                                       mViewMode, ct);
                    if(!v) continue;
                    const double cx = c->x();
                    const double cy = c->y();
                    double x;
                    double y;
                    if(dir == eWorldDirection::N) {
                        x = worldTileX - da*0.5 + cx + 0.25;
                        y = worldTileY - da*0.5 + cy + 0.25;
                    } else if(dir == eWorldDirection::E) {
                        x = viewTileX - da*0.5 + cy + 0.25;
                        y = viewTileY - da*0.5 - cx + 1.25;
                    } else if(dir == eWorldDirection::S) {
                        x = viewTileX - da*0.5 - cx + 1.25;
                        y = viewTileY - da*0.5 - cy + 1.25;
                    } else { // if(dir == eWorldDirection::W) {
                        x = viewTileX - da*0.5 - cy + 1.25;
                        y = viewTileY - da*0.5 + cx + 0.25;
                    }
                    if(!pyramid) {
                        const auto t = tile->topRotated<eTile>(dir);
                        const auto l = tile->leftRotated<eTile>(dir);
                        const auto r = tile->rightRotated<eTile>(dir);
                        const auto b = tile->bottomRotated<eTile>(dir);
                        const auto tl = tile->topLeftRotated<eTile>(dir);
                        const auto tr = tile->topRightRotated<eTile>(dir);
                        const auto bl = tile->bottomLeftRotated<eTile>(dir);
                        const auto br = tile->bottomRightRotated<eTile>(dir);
                        if(tl && tl->characterDoubleAltitude() > da) {
                            const double mult = 1 - cx;
                            const int tla = tl->characterDoubleAltitude();
                            const double fa = mult*(tla - da)*0.5;
                            x -= fa;
                            y -= fa;
                        } else if(tr && tr->characterDoubleAltitude() > da) {
                            const double mult = 1 - cy;
                            const int tra = tr->characterDoubleAltitude();
                            const double fa = mult*(tra - da)*0.5;
                            x -= fa;
                            y -= fa;
                        } else if(bl && bl->characterDoubleAltitude() > da) {
                            const double mult = cy;
                            const int bla = bl->characterDoubleAltitude();
                            const double fa = mult*(bla - da)*0.5;
                            x -= fa;
                            y -= fa;
                        } else if(br && br->characterDoubleAltitude() > da) {
                            const double mult = cx;
                            const int bra = br->characterDoubleAltitude();
                            const double fa = mult*(bra - da)*0.5;
                            x -= fa;
                            y -= fa;
                        } else if(t && t->characterDoubleAltitude() > da) {
                            const double mult = (1 - cx)*(1 - cy);
                            const int ta = t->characterDoubleAltitude();
                            const double fa = mult*(ta - da)*0.5;
                            x -= fa;
                            y -= fa;
                        } else if(l && l->characterDoubleAltitude() > da) {
                            const double mult = (1 - cx)*cy;
                            const int la = l->characterDoubleAltitude();
                            const double fa = mult*(la - da)*0.5;
                            x -= fa;
                            y -= fa;
                        } else if(r && r->characterDoubleAltitude() > da) {
                            const double mult = cx*(1 - cy);
                            const int ra = r->characterDoubleAltitude();
                            const double fa = mult*(ra - da)*0.5;
                            x -= fa;
                            y -= fa;
                        } else if(b && b->characterDoubleAltitude() > da) {
                            const double mult = cx*cy;
                            const int ba = b->characterDoubleAltitude();
                            const double fa = mult*(ba - da)*0.5;
                            x -= fa;
                            y -= fa;
                        }
                    }
                    const auto tex = c->getTexture(mTileSize);
                    const bool patrolerSelected =
                            mPatrolHighlightBuilding &&
                            c.get() == mPatrolHighlightBuilding->patroler();
                    const bool walkerSelected = [&]() {
                        if(!mWalkerBuilding) return false;
                        const auto ca = dynamic_cast<CartTransporterAction*>(c->action());
                        return ca && ca->src() == mWalkerBuilding.get();
                    }();
                    const bool charHighlighted = walkerSelected || patrolerSelected;
                    const auto drawCharTex = [&](const std::shared_ptr<eTexture>& t,
                                                 const double cx, const double cy,
                                                 const bool drawDot) {
                        if(!t) return;
                        const double offX = mTileH*t->offsetX()/30.;
                        const double offY = mTileH*t->offsetY()/30.;
                        const int dx = std::round(mDX + 0.5*(cx - cy)*mTileW - offX);
                        const int dy = std::round(mDY + 0.5*(cx + cy)*mTileH - offY);
                        if(charHighlighted) {
                            t->setColorMod(selectedWalkerColor.r,
                                           selectedWalkerColor.g,
                                           selectedWalkerColor.b);
                        } else if(hover) {
                            t->setColorMod(hr, hg, hb);
                        }
                        t->render(r, dx, dy, false);
                        if(charHighlighted || hover) t->clearColorMod();
                        if(charHighlighted && drawDot) {
                            trackingBoxes.push_back({dx, dy});
                        }
                        if(mViewMode == eViewMode::distribution &&
                           ct == eCharacterType::cartTransporter) {
                            const auto cart = static_cast<eCartTransporter*>(c.get());
                            const auto ca = dynamic_cast<CartTransporterAction*>(c->action());
                            if(cart->hasResource() && ca && ca->noDestination()) {
                                cartProblemBoxes.push_back({dx, dy});
                            }
                        }
                    };
                    if(tex) drawCharTex(tex, x, y, true);
                    if(c->hasSecondaryTexture()) {
                        const auto stex = c->getSecondaryTexture(mTileSize);
                        if(stex.fTex)
                            drawCharTex(stex.fTex, x + stex.fX, y + stex.fY, false);
                    }
//                        tex->clearColorMod();
//                      tp.drawTexture(x, y, tex);
                }
            }
        };

        const auto drawNumber = [&](const int id) {
            const auto tex = numbers[id % 10];
            tp.drawTexture(drawX - 1.65, drawY - 2.60, tex,
                           eAlignment::hcenter | eAlignment::top);
        };

        const auto drawPatrolGuides = [&]() {
            if(mPatrolBuilding) {
                using ePatrolGuides = std::vector<ePatrolGuide>;
                const auto drawPGS = [&](const ePatrolGuides& pgs) {
                    int i = 0;
                    for(const auto& pg : pgs) {
                        if(pg.fX == worldTileX && pg.fY == worldTileY) {
                            const bool bothDirections =
                                    mPatrolBuilding->bothDirections();
                            const bool invalid = !eVectorHelpers::contains(mPatrolPath, tile) &&
                                                 (!bothDirections ||
                                                  !eVectorHelpers::contains(mPatrolPath1, tile));
                            const auto& coll = builTexs.fSpawner;
                            const int texId = mAnimFrame % coll.size();
                            const auto& tex = coll.getTexture(texId);
                            if(invalid) tex->setColorMod(255, 125, 125);
                            //const auto& coll = builTexs.fPatrolGuides;
                            //const auto tex = coll.getTexture(14);
                            //tp.drawTexture(drawX, drawY, tex, eAlignment::top);
                            tp.drawTexture(drawX, drawY - 1, tex,
                                           eAlignment::hcenter | eAlignment::top);
                            drawNumber(i + 1);
                            if(invalid) tex->clearColorMod();
                            break;
                        }
                        i++;
                    }
                };
                const auto& pgs = mPatrolBuilding->patrolGuides();
                drawPGS(pgs);
            }
        };

        const auto drawSpawner = [&]() {
            if(mTem->visible()) {
                const auto& banners = tile->banners();
                for(const auto& b : banners) {
                    const auto& coll = builTexs.fSpawner;
                    const int texId = mAnimFrame % coll.size();
                    const auto& tex = coll.getTexture(texId);
                    tp.drawTexture(drawX, drawY - 1, tex,
                                   eAlignment::hcenter | eAlignment::top);
                    const int id = b->id();
                    drawNumber(id);

                    std::shared_ptr<eTexture> topTex;
                    switch(b->type()) {
                    case eBannerTypeS::none:
                        break;
                    case eBannerTypeS::boar:
                        topTex = builTexs.fBoarPoint;
                        break;
                    case eBannerTypeS::deer:
                        topTex = builTexs.fDeerPoint;
                        break;
                    case eBannerTypeS::landInvasion:
                    case eBannerTypeS::seaInvasion:
                        topTex = builTexs.fLandInvasionPoint;
                        break;
                    case eBannerTypeS::disembarkPoint:
                        topTex = builTexs.fDisembarkPoint;
                        break;
                    case eBannerTypeS::entryPoint:
                        topTex = builTexs.fEntryPoint;
                        break;
                    case eBannerTypeS::riverEntryPoint:
                        topTex = builTexs.fRiverEntryPoint;
                        break;
                    case eBannerTypeS::exitPoint:
                        topTex = builTexs.fExitPoint;
                        break;
                    case eBannerTypeS::riverExitPoint:
                        topTex = builTexs.fRiverExitPoint;
                        break;
                    case eBannerTypeS::monsterPoint:
                        topTex = builTexs.fMonsterPoint;
                        break;
                    case eBannerTypeS::disasterPoint:
                    case eBannerTypeS::landSlidePoint:
                        topTex = builTexs.fDisasterPoint;
                        break;
                    case eBannerTypeS::wolf:
                        topTex = builTexs.fWolfPoint;
                        break;
                    }
                    if(topTex) {
                        tp.drawTexture(drawX - 2.5, drawY - 3.5, topTex,
                                       eAlignment::hcenter | eAlignment::top);
                    }
                }
            }
        };

        const auto drawMissiles = [&]() {
            const auto& mss = tile->missiles();
            for(const auto& m : mss) {
                const auto type = m->type();
                if(type == eMissileType::racingHorse) continue;
                const bool isWave = type == eMissileType::wave ||
                                    type == eMissileType::lava ||
                                    type == eMissileType::dust;
                if(isWave) continue;
                const double h = m->height();
                const double mx = m->x();
                const double my = m->y();
                double x;
                double y;
                if(dir == eWorldDirection::N) {
                    x = viewTileX + mx + 0.25 - h;
                    y = viewTileY + my + 0.25 - h;
                } else if(dir == eWorldDirection::E) {
                    x = viewTileX + my + 0.25 - h;
                    y = viewTileY - mx + 1.25 - h;
                } else if(dir == eWorldDirection::S) {
                    x = viewTileX - mx + 1.25 - h;
                    y = viewTileY - my + 1.25 - h;
                } else { // if(dir == eWorldDirection::W) {
                    x = viewTileX - my + 1.25 - h;
                    y = viewTileY + mx + 0.25 - h;
                }
                const auto tex = m->getTexture(mTileSize);
                tp.drawTexture(x, y, tex);
            }
        };

        const auto drawWaves = [this, dir, boardWidth, boardHeight, &tp](eTile* const tile) {
            const int worldTileX = tile->x();
            const int worldTileY = tile->y();
            int viewTileX;
            int viewTileY;
            eTileHelper::tileIdToRotatedTileId(worldTileX, worldTileY,
                                               viewTileX, viewTileY, dir,
                                               boardWidth, boardHeight);
            const auto& mss = tile->missiles();
            for(const auto& m : mss) {
                const auto type = m->type();
                if(type == eMissileType::racingHorse) continue;
                const bool isWave = type == eMissileType::wave ||
                                    type == eMissileType::lava ||
                                    type == eMissileType::dust;
                if(!isWave) continue;
                const double h = m->height();
                const double mx = m->x();
                const double my = m->y();
                double x;
                double y;
                if(dir == eWorldDirection::N) {
                    x = viewTileX + mx + 0.25 - h;
                    y = viewTileY + my + 0.25 - h;
                } else if(dir == eWorldDirection::E) {
                    x = viewTileX + my + 0.25 - h;
                    y = viewTileY - mx + 1.25 - h;
                } else if(dir == eWorldDirection::S) {
                    x = viewTileX - mx + 1.25 - h;
                    y = viewTileY - my + 1.25 - h;
                } else { // if(dir == eWorldDirection::W) {
                    x = viewTileX - my + 1.25 - h;
                    y = viewTileY + mx + 0.25 - h;
                }
                const auto tex = m->getTexture(mTileSize);
                tp.drawTexture(x, y, tex);
            }
        };

        // takes everything by param (no tile/drawX refs) so it can be deferred and
        // replayed after the tile loop for enemy banners - keep it that way
        const auto drawBannerTextures = [this, &tp, &charTexs]
            (SoldierBanner* const b, const double drawX, const double drawY,
             const SDL_Color bnrMod) {
            const auto mod = [&](const std::shared_ptr<eTexture>& t) {
                t->setColorMod(bnrMod.r, bnrMod.g, bnrMod.b);
            };
            const auto unmod = [&](const std::shared_ptr<eTexture>& t) {
                t->clearColorMod();
            };
            {
                eGameTextures::loadBanners();
                const auto& rods = charTexs.fBannerRod;
                const auto& rod = rods.getTexture(0);
                mod(rod);
                tp.drawTexture(drawX, drawY - 1, rod,
                               eAlignment::hcenter | eAlignment::top);
                unmod(rod);
            }
            {
                const int id = b->id();
                const auto& bnrs = charTexs.fBanners;
                const auto& bnr = bnrs[id % bnrs.size()];
                int texId;
                if(b->selected()) {
                    texId = (mAnimFrame/5) % 6;
                } else {
                    texId = 6;
                }
                const auto& tex = bnr.getTexture(texId);
                mod(tex);
                tp.drawTexture(drawX - 1, drawY - 2.6, tex,
                               eAlignment::hcenter | eAlignment::top);
                unmod(tex);
            }
            {
                const auto type = b->type();
                const auto& tps = charTexs.fBannerTops;
                const auto& pTps = charTexs.fPoseidonBannerTops;
                const bool p = b->atlantean();
                if(!p ||
                   type == eBannerType::aresWarrior ||
                   type == eBannerType::amazon) {
                    int itype = -1;
                    if(type == eBannerType::aresWarrior) {
                        itype = 0;
                    } else if(type == eBannerType::amazon) {
                        itype = -1;
                    } else if(type != eBannerType::enemy) {
                        itype = static_cast<int>(type);
                    }
                    if(itype != -1) {
                        const auto& top = tps.getTexture(itype);
                        mod(top);
                        tp.drawTexture(drawX - 2.5, drawY -  3.5, top,
                                       eAlignment::hcenter | eAlignment::top);
                        unmod(top);
                    }
                } else {
                    int itype = -1;
                    if(type == eBannerType::horseman) {
                        itype = 0;
                    } else if(type == eBannerType::rockThrower) {
                        itype = 1;
                    } else if(type == eBannerType::hoplite) {
                        itype = 2;
                    }
                    if(itype != -1) {
                        const auto& top = pTps.getTexture(itype);
                        mod(top);
                        tp.drawTexture(drawX - 2.5, drawY -  3.5, top,
                                       eAlignment::hcenter | eAlignment::top);
                        unmod(top);
                    }
                }
            }
        };

        const auto drawBanners = [&]() {
            const auto b = tile->soldierBanner();
            if(!b) return;
            const bool aid = b->militaryAid();
            const bool enemy = b->type() == eBannerType::enemy;
            bool hover;
            if(mLeftPressed && mMovedSincePress) {
                hover = false;
                const auto selected = selectedTiles();
                for(const auto t : selected) {
                   if(t == tile) {
                       hover = true;
                       break;
                   }
                }
            } else {
                hover = worldTileX == mHoverTX && worldTileY == mHoverTY;
            }

            SDL_Color bnrMod{255, 255, 255, 255};
            if(hover) bnrMod = SDL_Color{175, 255, 255, 255};
            else if(enemy) bnrMod = SDL_Color{255, 55, 55, 255};
            else if(aid) bnrMod = SDL_Color{255, 125, 125, 255};

            if(enemy) {
                // capture by value (b stays alive, owned by board); run after the
                // scheduled-draw flush so enemy banners land on top of everything
                deferredEnemyBanners.push_back(
                    [=]() { drawBannerTextures(b, drawX, drawY, bnrMod); });
            } else {
                drawBannerTextures(b, drawX, drawY, bnrMod);
            }
        };

        if(tile) {
            const auto terrainBuilding = tile->underBuilding();
            const auto terrainBuildingType = tile->underBuildingType();
            bool flatSanct = false;
            if(terrainBuilding) {
                if(const auto sb = dynamic_cast<eSanctBuilding*>(terrainBuilding)) {
                    const bool pyramid = eBuilding::sPyramidBuilding(terrainBuildingType);
                    if(!pyramid) flatSanct = sb->progress() <= 0;
                }
            }
            const bool drawsCharactersInsteadOfTexture =
                eBuilding::sFlatBuilding(terrainBuildingType);
            const auto terr = tile->terrain();
            if(!terrainBuilding || flatSanct ||
               drawsCharactersInsteadOfTexture) {
                if(mViewMode == eViewMode::appeal && !terrainBuilding &&
                   !dontDrawAppeal(terr) && !tile->isElevationTile()) {
                    const auto& am = mBoard->appealMap();
                    const int ttdx = tile->dx();
                    const int ttdy = tile->dy();
                    const auto ae = am.enabled(ttdx, ttdy);
                    if(!ae) drawTerrain(tile);
                } else {
                    drawTerrain(tile);
                }
            }
        }
        //drawTerrain(tile);

        drawSelectedRoadPreview(tile);

        drawSheepGoat();
        drawPatrol();
        drawAppeal();

        if(tile->hasFish()) {
            const auto& fh = builTexs.fFish;
            const int t = mTime/30;
            const auto tex = fh.getTexture(t % fh.size());
            const auto a = eAlignment::right | eAlignment::top;
            tp.drawTexture(drawX + 1, drawY, tex, a);
        }
        if(tile->hasUrchin()) {
            const auto& fh = builTexs.fUrchin;
            const int t = mTime/30;
            const auto tex = fh.getTexture(t % fh.size());
            const auto a = eAlignment::bottom;
            tp.drawTexture(drawX + 0.5, drawY - 0.5, tex, a);
        }

        const auto drawBridge = [&]() {
            if(mode == eBuildingMode::bridge) {
                eGameTextures::loadBridge();
                const bool r = eVectorHelpers::contains(bridgetTs, tile);
                if(r) {
                    const int texId = bridgeRot ? 11 : 10;
                    const auto& tex = builTexs.fBridge.getTexture(texId);
                    if(bridgeValid) tex->setColorMod(0, 255, 0);
                    else tex->setColorMod(255, 0, 0);
                    tp.drawTexture(drawX + 0.5, drawY - 0.5, tex,
                                   eAlignment::hcenter | eAlignment::top);
                    tex->clearColorMod();
                }
            }
        };

        const auto drawCrosswalk = [&]() {
            if(mode == eBuildingMode::crosswalk) {
                const auto b = mBoard->buildingAt(mHoverTX, mHoverTY);
                if(b && b->type() == eBuildingType::hippodromePiece) {
                    bool red = false;
                    for(int dx = -1; dx <= 1; dx++) {
                        for(int dy = -1; dy <= 1; dy++) {
                            if(dx == 0 && dy == 0) continue;
                            const auto bb = mBoard->buildingAt(mHoverTX + dx, mHoverTY + dy);
                            if(bb && bb->type() == eBuildingType::road) {
                                const auto r = static_cast<eRoad*>(bb);
                                if(r->aboveHippodrome() == b) {
                                    red = true;
                                    break;
                                }
                            }
                        }
                    }
                    const auto h = static_cast<eHippodromePiece*>(b);
                    int id = h->id();
                    if(id == 0) {
                        id = 4;
                    } else if(id == 6) {
                        id = 2;
                    } else if(id != 2 && id != 4) {
                        return;
                    }
                    const auto& r = h->tileRect();
                    const auto rr = eTileHelper::toRotatedRect(
                        r, dir, boardWidth, boardHeight);
                    const int textureFitTileX = rr.x + rr.w - 1;
                    const int textureFitTileY = rr.y + rr.h - 1;
                    if(viewTileX != textureFitTileX || viewTileY != textureFitTileY) return;

                    const int sizeId = static_cast<int>(mTileSize);
                    const auto& builTexs = eGameTextures::buildings()[sizeId];
                    const auto& coll = builTexs.fHippodrome;
                    stdsptr<eTexture> tex;
                    int hx;
                    int hy;
                    const auto draw = [&]() {
                        if(tex) {
                            double drawX;
                            double drawY;
                            drawXY(hx, hy, drawX, drawY, 1, 1, a);
                            if(red) tex->setColorMod(255, 0, 0);
                            else tex->setColorMod(0, 255, 0);
                            tp.drawTexture(drawX + 0.5, drawY - 0.5, tex,
                                           eAlignment::hcenter | eAlignment::top);
                            tex->clearColorMod();
                        }
                    };
                    if(id == 2) {
                        bool reverse = false;
                        int texId1;
                        int texId2;
                        int texId3;
                        switch(dir) {
                        case eWorldDirection::N: {
                            texId1 = 11;
                            texId2 = 12;
                            texId3 = 13;
                        } break;
                        case eWorldDirection::E: {
                            texId1 = 8;
                            texId2 = 9;
                            texId3 = 10;
                            reverse = true;
                        } break;
                        case eWorldDirection::S: {
                            texId1 = 13;
                            texId2 = 12;
                            texId3 = 11;
                            reverse = true;
                        } break;
                        case eWorldDirection::W: {
                            texId1 = 10;
                            texId2 = 9;
                            texId3 = 8;
                        } break;
                        }

                        for(int x = reverse ? r.x + r.w - 1 : r.x;
                            reverse ? x >= r.x : x < r.x + r.w;
                            reverse ? x-- : x++) {
                            hx = x;
                            hy = mHoverTY;
                            if(x == r.x) {
                                tex = coll.getTexture(texId1);
                            } else if(x == r.x + r.w - 1) {
                                tex = coll.getTexture(texId3);
                            } else {
                                tex = coll.getTexture(texId2);
                            }
                            draw();
                        }
                    } else if(id == 4) {
                        bool reverse = false;
                        int texId1;
                        int texId2;
                        int texId3;
                        switch(dir) {
                        case eWorldDirection::N: {
                            texId1 = 10;
                            texId2 = 9;
                            texId3 = 8;
                        } break;
                        case eWorldDirection::E: {
                            texId1 = 11;
                            texId2 = 12;
                            texId3 = 13;
                        } break;
                        case eWorldDirection::S: {
                            texId1 = 8;
                            texId2 = 9;
                            texId3 = 10;
                            reverse = true;
                        } break;
                        case eWorldDirection::W: {
                            texId1 = 13;
                            texId2 = 12;
                            texId3 = 11;
                            reverse = true;
                        } break;
                        }

                        for(int y = reverse ? r.y + r.h - 1 : r.y;
                            reverse ? y >= r.y : y < r.y + r.h;
                            reverse ? y-- : y++) {
                            hx = mHoverTX;
                            hy = y;
                            if(y == r.y) {
                                tex = coll.getTexture(texId1);
                            } else if(y == r.y + r.h - 1) {
                                tex = coll.getTexture(texId3);
                            } else {
                                tex = coll.getTexture(texId2);
                            }
                            draw();
                        }
                    } else {
                        return;
                    }
                }
            }
        };
        const auto drawCrosswalkCharacters = [&]() {
            auto b = building;
            if(buildingType == eBuildingType::road) {
                const auto r = static_cast<eRoad*>(b);
                b = r->aboveHippodrome();
            }
            if(b && b->type() == eBuildingType::hippodromePiece) {
                const auto& r = b->tileRect();
                const auto rr = eTileHelper::toRotatedRect(
                    r, dir, boardWidth, boardHeight);
                const int textureFitTileX = rr.x + rr.w - 1;
                const int textureFitTileY = rr.y + rr.h - 1;
                if(viewTileX != textureFitTileX || viewTileY != textureFitTileY) return;

                switch(dir) {
                case eWorldDirection::N: {
                    for(int y = r.y; y < r.y + r.h; y++) {
                        for(int x = r.x; x < r.x + r.w; x++) {
                            const auto tile = mBoard->tile(x, y);
                            drawCharacters(tile, false, true);
                        }
                    }
                } break;
                case eWorldDirection::E: {
                    for(int x = r.x + r.w - 1; x >= r.x; x--) {
                        for(int y = r.y; y < r.y + r.h; y++) {
                            const auto tile = mBoard->tile(x, y);
                            drawCharacters(tile, false, true);
                        }
                    }
                } break;
                case eWorldDirection::S: {
                    for(int y = r.y + r.h - 1; y >= r.y; y--) {
                        for(int x = r.x + r.w - 1; x >= r.x; x--) {
                            const auto tile = mBoard->tile(x, y);
                            drawCharacters(tile, false, true);
                        }
                    }
                } break;
                case eWorldDirection::W: {
                    for(int x = r.x; x < r.x + r.w; x++) {
                        for(int y = r.y + r.h - 1; y >= r.y; y--) {
                            const auto tile = mBoard->tile(x, y);
                            drawCharacters(tile, false, true);
                        }
                    }
                } break;
                }
            }
        };

        drawBridge();
        drawPatrolGuides();
        drawSpawner();

        if(buildingType == eBuildingType::templeTile) {
            const auto t = static_cast<eTempleTileBuilding*>(building);
            const int tid = t ? t->id() : 0;
            if(tid >= 10) {
                const auto s = t ? t->monument() : nullptr;
                const int f = s && s->finished();
                if(f) {
                    const auto& coll = builTexs.fSanctuaryFire;
                    const int textureTime = mAnimFrame/4;
                    const int texId = textureTime % coll.size();
                    const auto& tex = coll.getTexture(texId);
                    tp.drawTexture(drawX + 0.5, drawY - 0.5, tex, eAlignment::bottom);
                }
            }
        }

        const auto r = p.renderer();
        enum class eTileClipSide {
            left, right
        };

        const auto clipTileRect = [&](const eTileClipSide side) {
            SDL_Rect clipRect;
            clipRect.y = -10000;
            clipRect.h = 20000;
            switch(side) {
            case eTileClipSide::left: {
                clipRect.x = mDX + (viewTileX - viewTileY - 1)*mTileW/2;
                clipRect.w = 10000;
            } break;
            case eTileClipSide::right: {
                clipRect.x = mDX + (viewTileX - viewTileY - 1)*mTileW/2 - 10000;
                clipRect.w = mTileW + 10000;
            } break;
            }
            SDL_RenderSetClipRect(r, &clipRect);
        };

        enum class eCharRenderOrder {
            x0y1x1y0,
            x1y1
        };

        const auto tileCharRenderOrder = [dir](const eTile* tile) {
            const auto tileDrawsCharactersInsteadOfTexture =
                [](const eTile* const tile) {
                const auto buildingType = tile->underBuildingType();
                const bool drawsCharactersInsteadOfTexture =
                    eBuilding::sFlatBuilding(buildingType);
                return drawsCharactersInsteadOfTexture;
            };
            {
                const auto t_x0y1 = tile->bottomLeftRotated<eTile>(dir);
                if(t_x0y1) {
                    const bool drawsCharactersInsteadOfTexture =
                        tileDrawsCharactersInsteadOfTexture(t_x0y1);
                    if(!drawsCharactersInsteadOfTexture) {
                        return eCharRenderOrder::x0y1x1y0;
                    }
                }
            }
            {
                const auto t_x1y0 = tile->bottomRightRotated<eTile>(dir);
                if(t_x1y0) {
                    const bool drawsCharactersInsteadOfTexture =
                        tileDrawsCharactersInsteadOfTexture(t_x1y0);
                    if(!drawsCharactersInsteadOfTexture) {
                        return eCharRenderOrder::x0y1x1y0;
                    }
                }
            }
            return eCharRenderOrder::x1y1;
        };
        bool lastTile = false;
        if(dir == eWorldDirection::N) {
            lastTile = dty >= mBoard->height() - 2;
        } else if(dir == eWorldDirection::E) {
            lastTile = dtx <= 1;
        } else if(dir == eWorldDirection::S) {
            lastTile = dty <= 1;
        } else if(dir == eWorldDirection::W) {
            lastTile = dtx >= mBoard->width() - 2;
        }
        {
            const auto tt = tile->tileRelRotated<eTile>(-3, -3, dir);
            if(tt) {
                drawCharacters(tt, true, false);
            }
            if(lastTile) {
                for(int i = 0; i < 3; i++) {
                    const auto tt = tile->tileRelRotated<eTile>(-i, -i, dir);
                    if(tt) {
                        drawCharacters(tt, true, false);
                    }
                }
            }
        }
        {
            const auto t = tile->topRotated<eTile>(dir);
            if(t) {
                const auto order = tileCharRenderOrder(t);
                if(order == eCharRenderOrder::x1y1) {
                    drawCharacters(t, false, false);
                }
            }
        }
        {
            const auto tl = tile->topLeftRotated<eTile>(dir);
            if(tl) {
                const auto order = tileCharRenderOrder(tl);
                if(order == eCharRenderOrder::x0y1x1y0) {
                    clipTileRect(eTileClipSide::left);
                    drawCharacters(tl, false, false);
                    SDL_RenderSetClipRect(r, nullptr);
                }
            }
        }
        {
            const auto tr = tile->topRightRotated<eTile>(dir);
            if(tr) {
                const auto order = tileCharRenderOrder(tr);
                if(order == eCharRenderOrder::x0y1x1y0) {
                    clipTileRect(eTileClipSide::right);
                    drawCharacters(tr, false, false);
                    SDL_RenderSetClipRect(r, nullptr);
                }
            }
        }

        drawBanners();

        buildingDrawer(tile);
        drawCrosswalk();
        drawCrosswalkCharacters();

        drawMissiles();

        if(mBoard->duringTidalWave() ||
           mBoard->duringLavaFlow() ||
           mBoard->duringLandSlide()) {
            const auto tt = tile->tileRelRotated<eTile>(-2, -2, dir);
            if(tt) {
                drawWaves(tt);
            }
            if(lastTile) {
                for(int i = 0; i < 2; i++) {
                    const auto tt = tile->tileRelRotated<eTile>(-i, -i, dir);
                    if(tt) {
                        drawWaves(tt);
                    }
                }
            }
        }

        if(mLeftPressed && mMovedSincePress &&
           mGm->visible() && mGm->mode() == eBuildingMode::none) {
            const int x = mPressedX > mHoverX ? mHoverX : mPressedX;
            const int y = mPressedY > mHoverY ? mHoverY : mPressedY;
            const int w = abs(mPressedX - mHoverX);
            const int h = abs(mPressedY - mHoverY);
            SDL_Rect selRect{x - mDX, y - mDY, w, h};
            p.drawRect(selRect, SDL_Color{0, 255, 0, 255}, 1);
        } });

    tp.handleScheduledDraw();

    // enemy banners drawn after flush so they sit on top of everything
    for(const auto& d : deferredEnemyBanners) d();

    paintInvasionDebugTargets(*mBoard, mViewedCityId, p,
                              mTileW, mTileH, mAnimFrame);

    if (mRightFormationFacing)
    {
        const auto banners = SoldierBanner::sPlayerBanners(
            mBoard->selectedSoldiers(), mBoard->personPlayer());
        if (!banners.empty())
        {
            int lineDX;
            int lineDY;
            rightDragFormationLine(lineDX, lineDY);
            const int facing = rightDragFacing();
            const int dist = 3;
            const auto slots = SoldierBanner::sFormationPositions(
                banners, mPressedTX, mPressedTY, facing, lineDX, lineDY, dist);

            eGameTextures::loadBanners();
            const auto drawGhostTile = [&](eTile* const tile,
                                           const bool valid)
            {
                if (!tile) return;
                int viewTileX;
                int viewTileY;
                eTileHelper::tileIdToRotatedTileId(tile->x(), tile->y(),
                                                   viewTileX, viewTileY, dir,
                                                   boardWidth, boardHeight);
                const int ta = mDrawElevation ? tile->altitude() : 0;
                const int sx = mDX + (viewTileX - viewTileY)*mTileW/2;
                const int sy = mDY + (viewTileX + viewTileY - 2*ta)*mTileH/2;
                const SDL_Color color = valid ?
                    SDL_Color{220, 255, 80, 200} :
                    SDL_Color{255, 80, 80, 220};
                std::vector<SDL_Point> pts{
                    {sx, sy},
                    {sx + mTileW/2, sy + mTileH/2},
                    {sx, sy + mTileH},
                    {sx - mTileW/2, sy + mTileH/2},
                    {sx, sy}
                };
                p.drawPolygon(pts, color);
            };
            const auto drawGhostBanner = [&](SoldierBanner* const b,
                                             eTile* const tile,
                                             const bool valid)
            {
                if (!b || !tile) return;
                double drawX;
                double drawY;
                const int ta = mDrawElevation ? tile->altitude() : 0;
                drawXY(tile->x(), tile->y(), drawX, drawY, 1, 1, ta);
                const auto tint = valid ?
                    SDL_Color{220, 255, 80, 140} :
                    SDL_Color{255, 80, 80, 150};

                const auto& rods = charTexs.fBannerRod;
                const auto& rod = rods.getTexture(0);
                rod->setColorMod(tint.r, tint.g, tint.b);
                rod->setAlpha(tint.a);
                tp.drawTexture(drawX, drawY - 1, rod,
                               eAlignment::hcenter | eAlignment::top);
                rod->clearAlphaMod();
                rod->clearColorMod();

                const auto& bnrs = charTexs.fBanners;
                const auto& bnr = bnrs[b->id() % bnrs.size()];
                const auto& tex = bnr.getTexture(6);
                tex->setColorMod(tint.r, tint.g, tint.b);
                tex->setAlpha(tint.a);
                tp.drawTexture(drawX - 1, drawY - 2.6, tex,
                               eAlignment::hcenter | eAlignment::top);
                tex->clearAlphaMod();
                tex->clearColorMod();

                const auto type = b->type();
                const bool poseidon = b->atlantean();
                int itype = -1;
                const eTextureCollection* tops = nullptr;
                if(!poseidon ||
                   type == eBannerType::aresWarrior ||
                   type == eBannerType::amazon) {
                    tops = &charTexs.fBannerTops;
                    if(type == eBannerType::aresWarrior) {
                        itype = 0;
                    } else if(type != eBannerType::amazon &&
                              type != eBannerType::enemy) {
                        itype = static_cast<int>(type);
                    }
                } else {
                    tops = &charTexs.fPoseidonBannerTops;
                    if(type == eBannerType::horseman) {
                        itype = 0;
                    } else if(type == eBannerType::rockThrower) {
                        itype = 1;
                    } else if(type == eBannerType::hoplite) {
                        itype = 2;
                    }
                }
                if(tops && itype != -1) {
                    const auto& top = tops->getTexture(itype);
                    top->setColorMod(tint.r, tint.g, tint.b);
                    top->setAlpha(tint.a);
                    tp.drawTexture(drawX - 2.5, drawY -  3.5, top,
                                   eAlignment::hcenter | eAlignment::top);
                    top->clearAlphaMod();
                    top->clearColorMod();
                }
            };

            for(const auto& slot : slots) {
                const auto tile = mBoard->tile(slot.tx, slot.ty);
                const bool valid = tile &&
                    tile->cityId() == slot.banner->onCityId() &&
                    tile->walkable() &&
                    (!tile->soldierBanner() || tile->soldierBanner() == slot.banner);
                drawGhostTile(tile, valid);
                drawGhostBanner(slot.banner, tile, valid);
            }
        }

        const int sx = mPressedX - mDX;
        const int sy = mPressedY - mDY;
        const int ex = mHoverX - mDX;
        const int ey = mHoverY - mDY;
        const double dx = ex - sx;
        const double dy = ey - sy;
        const double len = std::sqrt(dx*dx + dy*dy);
        if (len > 0)
        {
            const double ux = dx/len;
            const double uy = dy/len;
            const int head = 12;
            const SDL_Color color{255, 240, 64, 255};
            std::vector<SDL_Point> shaft{{sx, sy}, {ex, ey}};
            p.drawPolygon(shaft, color);
            std::vector<SDL_Point> leftHead{
                {ex, ey},
                {int(ex - ux*head - uy*head/2), int(ey - uy*head + ux*head/2)}
            };
            std::vector<SDL_Point> rightHead{
                {ex, ey},
                {int(ex - ux*head + uy*head/2), int(ey - uy*head - ux*head/2)}
            };
            p.drawPolygon(leftHead, color);
            p.drawPolygon(rightHead, color);
        }
    }

    if (mPatrolBuilding)
    {
        const auto &pgs = mPatrolBuilding->patrolGuides();
        if (!pgs.empty())
        {
            const auto t = mPatrolBuilding->centerTile();
            const int worldTileX = t->x();
            const int worldTileY = t->y();
            int viewTileX;
            int viewTileY;
            eTileHelper::tileIdToRotatedTileId(worldTileX, worldTileY,
                                               viewTileX, viewTileY, dir,
                                               boardWidth, boardHeight);
            const int ta = t->altitude();
            std::vector<SDL_Point> polygon;
            polygon.reserve(pgs.size() + 2);
            polygon.push_back({viewTileX - ta, viewTileY - ta});
            for (const auto &pg : pgs)
            {
                const int worldTileX = pg.fX;
                const int worldTileY = pg.fY;
                int viewTileX;
                int viewTileY;
                eTileHelper::tileIdToRotatedTileId(worldTileX, worldTileY,
                                                   viewTileX, viewTileY, dir,
                                                   boardWidth, boardHeight);
                const auto t = mBoard->tile(worldTileX, worldTileY);
                const int ta = t->altitude();
                polygon.push_back({viewTileX - ta, viewTileY - ta});
            }
            polygon.push_back({viewTileX - ta, viewTileY - ta});
            tp.drawPolygon(polygon, {0, 0, 0, 255});
        }
    }


    paintBuildPreview(tp, p, trrTexs, builTexs, ppid, mode, dir,
                      boardWidth, boardHeight, bridgeValid, bridgetTs,
                      sMinX, sMaxX, sMinY, sMaxY);
    for (const auto &pos : trackingBoxes)
    {
        const int dx = pos.first;
        const int dy = pos.second;
        constexpr int ds = 18;
        const SDL_Rect dot{dx - ds / 2, dy - ds, ds, ds};
        auto r = p.renderer();
        SDL_SetRenderDrawColor(r, selectedWalkerColor.r,
                               selectedWalkerColor.g,
                               selectedWalkerColor.b, 255);
        SDL_RenderFillRect(r, &dot);
        SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
        SDL_RenderDrawRect(r, &dot);
    }
    for (const auto &pos : cartProblemBoxes)
    {
        const int dx = pos.first;
        const int dy = pos.second;
        constexpr int ds = 18;
        const SDL_Rect box{dx - ds / 2, dy - 2 * ds, ds, ds};
        auto r = p.renderer();
        SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
        SDL_RenderFillRect(r, &box);
        SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
        SDL_RenderDrawRect(r, &box);
    }

    for(const auto& dot : templeDebugDots) {
        tp.fillRectCenter(dot.fDrawX, dot.fDrawY,
                          dot.fSize, dot.fSize, dot.fColor);
    }


    SDL_SetRenderTarget(r, nullptr);
    SDL_RenderSetClipRect(r, nullptr);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    const int srcW = std::round(w / mZoom);
    const int srcH = std::round(h / mZoom);
    const int srcX = (w - srcW) / 2;
    const int srcY = (h - srcH) / 2;
    const SDL_Rect srcRect{srcX, srcY, srcW, srcH};
    const SDL_Rect dstRect{0, 0, w, h};
    SDL_RenderCopy(r, mWorldTex->tex(), &srcRect, &dstRect);

    {
        const char* letters[] = {"N", "W", "S", "E"};
        const int idx = static_cast<int>(dir);
        const auto letter = letters[idx % 4];
        const auto tex = std::make_shared<eTexture>();
        auto* font = eFonts::defaultFont(45);
        if(font) {
            tex->loadText(r, letter, eFontColor::light, *font);
            SDL_SetTextureBlendMode(tex->tex(), SDL_BLENDMODE_BLEND);
            tex->setAlpha(140);
            const int sideW = mGm ? mGm->width() : 0;
        const int px = w - sideW - tex->width() - 20;
            const int topH = mTopBar ? mTopBar->height() : 0;
            const int py = topH + 10;
            tex->render(r, px, py);
        }
    }
}
