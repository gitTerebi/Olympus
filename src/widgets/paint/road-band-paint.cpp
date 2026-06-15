#include "widgets/game-widget.h"

#include "characters/actions/epatrolmoveaction.h"
#include "characters/actions/walkable/walkable-object.h"
#include "engine/epathfinder.h"
#include "engine/etile.h"
#include "engine/game-board.h"
#include "buildings/eagorabase.h"
#include "buildings/epatrolbuildingbase.h"
#include "buildings/eroad.h"
#include "enumbers.h"
#include "widgets/etilepainter.h"
#include "textures/terrain-textures.h"

#include <algorithm>
#include <map>

namespace {

constexpr int kMaxPreviewFreq = 16;

bool previewRoadTile(eTile* const tile, const bool includeAgoraRoads)
{
    if (!tile) return false;
    if (includeAgoraRoads) return tile->hasRoad();
    if (tile->underBuildingType() == eBuildingType::avenue) return true;
    if (!tile->hasRoad()) return false;
    const auto r = static_cast<eRoad*>(tile->underBuilding());
    return !r->underAgora();
}

void addPreviewTile(eTile* const tile, std::map<eTile*, int>& path,
                    const bool includeAgoraRoads)
{
    if (!previewRoadTile(tile, includeAgoraRoads)) return;
    auto& freq = path[tile];
    freq = std::min(freq + 1, kMaxPreviewFreq);
}

}

void GameWidget::drawRoadFootprint(eTile* const tile, const SDL_Color color,
                                   eTilePainter& tp,
                                   const TerrainTextures& trrTexs)
{
    if (!tile) return;
    const int worldTileX = tile->x();
    const int worldTileY = tile->y();
    const int altitude = mDrawElevation ? tile->altitude() : 0;
    double drawX;
    double drawY;
    drawXY(worldTileX, worldTileY, drawX, drawY, 1, 1, altitude);
    const auto& tex = trrTexs.fBuildingBase;
    tex->setColorMod(color.r, color.g, color.b);
    tex->setAlpha(color.a);
    tp.drawTexture(drawX, drawY, tex, eAlignment::top);
    tex->clearAlphaMod();
    tex->clearColorMod();
}

bool GameWidget::isRoadBandTile(eTile* const tile)
{
    if (!tile) return false;
    if (tile->underBuildingType() == eBuildingType::avenue) return true;
    if (!tile->hasRoad()) return false;
    const auto r = static_cast<eRoad*>(tile->underBuilding());
    return !r->underAgora();
}

void GameWidget::addPatrolWalkerPreview(eTile* const start,
                                  eTile* const home,
                                  eRoadPreviewPath& path,
                                  const std::shared_ptr<WalkableObject>& walkable,
                                  const bool includeAgoraRoads,
                                  const SDL_Rect* const despawnRect)
{
    if(!start || !home || !walkable) return;
    const auto addReturnPath = [&](eTile* const from,
                                   const std::vector<eTile*>& outbound) {
        if(!from || from == home) return;
        auto returnWalkable = walkable;
        if(despawnRect && !SDL_RectEmpty(despawnRect)) {
            returnWalkable = WalkableObject::sCreateRect(*despawnRect,
                                                         returnWalkable);
        } else if(const auto b = home->underBuilding()) {
            returnWalkable = WalkableObject::sCreateRect(b, returnWalkable);
        }
        const int hx = home->x();
        const int hy = home->y();
        const auto final = [hx, hy](eTileBase* const t) -> bool {
            if(!t) return false;
            return t->x() == hx && t->y() == hy;
        };
        ePathFinder p([&](eTileBase* const t) {
            return returnWalkable->walkable(t);
        }, final);
        const auto cid = from->cityId();
        auto rect = mBoard->boardCityTileBRect(cid);
        if(SDL_RectEmpty(&rect)) rect = {0, 0, mBoard->width(), mBoard->height()};
        const bool r = p.findPath(rect, from, 1000, true,
                                  mBoard->width(), mBoard->height());
        if(!r) {
            for(auto it = outbound.rbegin(); it != outbound.rend(); ++it) {
                addPreviewTile(*it, path, includeAgoraRoads);
            }
            return;
        }
        std::vector<eTile*> returnPath;
        if(!p.extractPath(returnPath, *mBoard)) {
            for(auto it = outbound.rbegin(); it != outbound.rend(); ++it) {
                addPreviewTile(*it, path, includeAgoraRoads);
            }
            return;
        }
        for(const auto tile : returnPath) {
            addPreviewTile(tile, path, includeAgoraRoads);
        }
    };

    eDirectionTimes useTimes;
    constexpr int previewWalkerSamples = 8;
    for(int sample = 0; sample < previewWalkerSamples; sample++) {
        auto tile = start;
        auto o = static_cast<eOrientation>(2*(sample % 4));
        std::vector<eTile*> outbound;
        for(int time = 0; time <= eNumbers::sPatrolerMaxDistance && tile;
            time++) {
            addPreviewTile(tile, path, includeAgoraRoads);
            outbound.push_back(tile);
            const unsigned seed =
                1103515245u*static_cast<unsigned>(sample + 1) +
                12345u*static_cast<unsigned>(time + 1) +
                97u*static_cast<unsigned>(tile->x()) +
                193u*static_cast<unsigned>(tile->y()) +
                17u*static_cast<unsigned>(o);
            const auto picker = [seed](const std::vector<eOrientation>& options,
                                       const eOrientation) {
                const int id = seed % options.size();
                return options[id];
            };
            const int simTime = sample*(eNumbers::sPatrolerMaxDistance + 1) +
                                time + 1;
            const auto state = ePatrolMoveAction::sNextTurn(
                tile, true, *walkable, useTimes, o, simTime, picker);
            if(state != eCharacterActionState::running) break;
            const auto next = tile->neighbour<eTile>(o);
            if(!next || !walkable->walkable(next)) break;
            tile = next;
        }
        addReturnPath(tile, outbound);
    }
}

void GameWidget::addPatrolBuildingRoadPreview(
        ePatrolBuildingBase* const building,
        eRoadPreviewPath& path,
        eTile*& spawnTile)
{
    spawnTile = nullptr;
    if(!building) return;

    const auto buildingType = building->type();
    const bool agora = buildingType == eBuildingType::commonAgora ||
                       buildingType == eBuildingType::grandAgora;

    eTile* home = building->centerTile();
    const auto rect = building->tileRect();
    if(!home && !SDL_RectEmpty(&rect)) {
        home = mBoard->tile(rect.x + rect.w/2, rect.y + rect.h/2);
    }
    std::shared_ptr<WalkableObject> walkable;
    bool includeAgoraRoads = false;
    const SDL_Rect* despawnRect = &rect;

    if(agora) {
        const auto ab = static_cast<eAgoraBase*>(building);
        spawnTile = ab->agoraRoadExit();
        walkable = WalkableObject::sCreateRoadblockNoAgora();
        includeAgoraRoads = true;
        despawnRect = nullptr;
    } else {
        const auto roads = building->surroundingRoad(false, true);
        if(roads.empty()) return;
        spawnTile = roads.front();
        walkable = WalkableObject::sCreateRoadblock();
        if(building->bothDirections() && roads.back() != spawnTile) {
            addPatrolWalkerPreview(roads.back(), home, path, walkable,
                                   false, despawnRect);
        }
    }

    if(!spawnTile || !home || !walkable) return;
    addPatrolWalkerPreview(spawnTile, home, path, walkable,
                           includeAgoraRoads, despawnRect);
}

void GameWidget::addPathBands(const std::vector<eTile*>& tiles,
                              eRoadPreviewPath& path)
{
    for (const auto tile : tiles) {
        if (!isRoadBandTile(tile)) continue;
        auto& freq = path[tile];
        freq = std::min(freq + 1, 8);
    }
}

eTile* GameWidget::firstPathRoad(const std::vector<eTile*>& tiles)
{
    for (const auto tile : tiles) {
        if (isRoadBandTile(tile)) return tile;
    }
    return nullptr;
}

eTile* GameWidget::lastPathRoad(const std::vector<eTile*>& tiles)
{
    for (auto it = tiles.rbegin(); it != tiles.rend(); ++it) {
        if (isRoadBandTile(*it)) return *it;
    }
    return nullptr;
}

SDL_Color GameWidget::roadBandColor(const int freq)
{
    const int f = std::clamp(freq, 1, kMaxPreviewFreq);
    const int hot = (f - 1)*255/(kMaxPreviewFreq - 1);
    return SDL_Color{
        static_cast<Uint8>(0xd8 - hot*0x90/255),
        static_cast<Uint8>(0xf0 - hot*0x78/255),
        0xff,
        static_cast<Uint8>(0x34 + hot*0x64/255)
    };
}

void GameWidget::drawRoadBandTile(eTile* const tile,
                                  eTile* const start,
                                  const eRoadPreviewPath& path,
                                  eTilePainter& tp,
                                  const TerrainTextures& trrTexs)
{
    if (!tile || !start) return;
    if (tile == start) {
        drawRoadFootprint(tile, SDL_Color{80, 255, 80, 220}, tp, trrTexs);
        return;
    }
    const auto it = path.find(tile);
    if (it == path.end()) return;
    drawRoadFootprint(tile, roadBandColor(it->second), tp, trrTexs);
}

void GameWidget::drawRoadPreview(const eRoadPreviewPath& path,
                                 eTile* const spawnTile,
                                 eTilePainter& tp,
                                 const TerrainTextures& trrTexs)
{
    for (const auto& p : path) {
        drawRoadBandTile(p.first, spawnTile, path, tp, trrTexs);
    }
    if (spawnTile && !path.count(spawnTile)) {
        drawRoadBandTile(spawnTile, spawnTile, path, tp, trrTexs);
    }
}

void GameWidget::drawRoadBands(const std::vector<eTile*>& roads,
                               eTilePainter& tp,
                               const TerrainTextures& trrTexs)
{
    if (roads.empty()) return;
    const auto start = roads.front();
    eRoadPreviewPath path;
    const auto walkable = WalkableObject::sCreateRoadblock();
    addPatrolWalkerPreview(start, start, path, walkable);
    drawRoadPreview(path, start, tp, trrTexs);
}
