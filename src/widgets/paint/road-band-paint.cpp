#include "widgets/game-widget.h"

#include "characters/actions/walkable/walkable-object.h"
#include "engine/etile.h"
#include "buildings/eroad.h"
#include "enumbers.h"
#include "evectorhelpers.h"
#include "widgets/etilepainter.h"
#include "textures/eterraintextures.h"

#include <array>
#include <map>

void GameWidget::drawRoadFootprint(eTile* const tile, const SDL_Color color,
                                   eTilePainter& tp,
                                   const eTerrainTextures& trrTexs)
{
    if (!tile) return;
    const int tx = tile->x();
    const int ty = tile->y();
    const int a = mDrawElevation ? tile->altitude() : 0;
    double rx;
    double ry;
    drawXY(tx, ty, rx, ry, 1, 1, a);
    const auto& tex = trrTexs.fBuildingBase;
    tex->setColorMod(color.r, color.g, color.b);
    tex->setAlpha(color.a);
    tp.drawTexture(rx, ry, tex, eAlignment::top);
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

void GameWidget::addRoamerPreview(eTile* const start,
                                  eRoadPreviewPath& path,
                                  const std::shared_ptr<WalkableObject>& walkable)
{
    using eUseTimes = std::map<eTile*, std::array<int, 8>>;
    for (int i = 0; i < 4; i++) {
        eUseTimes useTimes;
        auto tile = start;
        auto prev = static_cast<eTile*>(nullptr);
        auto o = static_cast<eOrientation>(2 * i);
        for (int time = 1;
             time < eNumbers::sPatrolerMaxDistance && tile;
             time++)
        {
            const bool agRoad = tile->hasRoad() &&
                                static_cast<eRoad*>(tile->underBuilding())->underAgora();
            if(!agRoad) {
                auto& freq = path[tile];
                freq = std::min(freq + 1, 8);
            }
            const auto valid = [&](eTileBase* const t) {
                const auto tt = static_cast<eTile*>(t);
                return walkable->walkable(tt) &&
                       tt->neighbour<eTile>(o) != prev;
            };
            auto options = tile->diagonalNeighbours(valid);
            if (options.empty()) {
                o = !o;
            } else {
                int minUse = __INT_MAX__;
                std::vector<eOrientation> best;
                auto& uses = useTimes[tile];
                for (const auto& opt : options) {
                    const auto oo = opt.first;
                    const int used = uses[static_cast<int>(oo)];
                    if (used < minUse) {
                        minUse = used;
                        best.clear();
                        best.push_back(oo);
                    } else if (used == minUse) {
                        best.push_back(oo);
                    }
                }
                if (!eVectorHelpers::contains(best, o) && !best.empty()) {
                    o = best.front();
                }
            }
            const auto next = tile->neighbour<eTile>(o);
            if (!walkable->walkable(next)) break;
            useTimes[tile][static_cast<int>(o)] = time;
            useTimes[next][static_cast<int>(!o)] = time;
            prev = tile;
            tile = next;
        }
    }
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
    return freq > 1 ? SDL_Color{0x00, 0x00, 0x88, 0x88}
                    : SDL_Color{0x33, 0x77, 0xff, 0x66};
}

void GameWidget::drawRoadBandTile(eTile* const tile,
                                  eTile* const start,
                                  eTile* const ret,
                                  const eRoadPreviewPath& path,
                                  eTilePainter& tp,
                                  const eTerrainTextures& trrTexs)
{
    if (!tile || !start) return;
    if (tile == start && tile == ret) {
        drawRoadFootprint(tile, SDL_Color{80, 80, 255, 220}, tp, trrTexs);
        return;
    }
    if (tile == start) {
        drawRoadFootprint(tile, SDL_Color{80, 255, 80, 220}, tp, trrTexs);
        return;
    }
    if (tile == ret) {
        drawRoadFootprint(tile, SDL_Color{255, 80, 80, 220}, tp, trrTexs);
        return;
    }
    const auto it = path.find(tile);
    if (it == path.end()) return;
    drawRoadFootprint(tile, roadBandColor(it->second), tp, trrTexs);
}

void GameWidget::drawRoadBands(const std::vector<eTile*>& roads,
                               eTilePainter& tp,
                               const eTerrainTextures& trrTexs)
{
    if (roads.empty()) return;
    const auto start = roads.front();
    const auto ret = roads.back();
    eRoadPreviewPath path;
    const auto walkable = WalkableObject::sCreateRoadblock();
    addRoamerPreview(start, path, walkable);
    for (const auto& p : path) {
        drawRoadBandTile(p.first, start, ret, path, tp, trrTexs);
    }
    if (!path.count(start))
        drawRoadBandTile(start, start, ret, path, tp, trrTexs);
    if (ret != start && !path.count(ret))
        drawRoadBandTile(ret, start, ret, path, tp, trrTexs);
}
