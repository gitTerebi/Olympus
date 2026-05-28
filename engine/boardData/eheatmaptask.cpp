#include "eheatmaptask.h"

#include "../thread/ethreadboard.h"

#include <array>

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

template <typename Tile>
Tile* avenueRoad(Tile* const tile) {
    if(!tile) return nullptr;
    const auto tl = tile->template topLeft<Tile>();
    if(tl && tl->hasRoad()) return tl;
    const auto br = tile->template bottomRight<Tile>();
    if(br && br->hasRoad()) return br;
    const auto bl = tile->template bottomLeft<Tile>();
    if(bl && bl->hasRoad()) return bl;
    const auto tr = tile->template topRight<Tile>();
    if(tr && tr->hasRoad()) return tr;
    return nullptr;
}

template <typename Tile, typename Add>
void addAvenueHeat(Tile* const avenueTile, const Add& add) {
    const auto road = avenueRoad(avenueTile);
    if(!avenueTile || !road) return;
    const int ux = road->x() - avenueTile->x();
    const int uy = road->y() - avenueTile->y();
    const int vx = -uy;
    const int vy = ux;
    const auto opposite = road->template tileRel<Tile>(ux, uy);
    const bool boulevard = opposite &&
        opposite->underBuildingType() == eBuildingType::avenue;
    if(boulevard) {
        const bool first = avenueTile->x() < opposite->x() ||
            (avenueTile->x() == opposite->x() && avenueTile->y() < opposite->y());
        if(!first) return;
    }
    const auto addCell = [&](const AvenueHeatCell& cell) {
        const int tx = cell.x*ux + cell.y*vx;
        const int ty = cell.x*uy + cell.y*vy;
        const auto target = avenueTile->template tileRel<Tile>(tx, ty);
        if(!target) return;
        add(target, cell.value);
    };
    if(boulevard) {
        for(const auto& cell : kBoulevardHeat) addCell(cell);
    } else {
        for(const auto& cell : kAvenueHeat) addCell(cell);
    }
}

}

eHeatMapTask::eHeatMapTask(const eCityId cid,
                           const SDL_Rect& bRect,
                           const eHeatGetter& heatGetter,
                           const eFunc& finish) :
    eTask(cid), mHeatGetter(heatGetter), mFinish(finish),
    mBRect(bRect) {
    setRelevance(eStateRelevance::buildings);
}

void eHeatMapTask::sRun(eThreadBoard& board,
                        const eCityId cid,
                        const SDL_Rect& bRect,
                        const eHeatGetter& heatGetter,
                        eHeatMap& map) {
    map.initialize(bRect.x, bRect.y, bRect.w, bRect.h);
    for(int tx = bRect.x; tx < bRect.x + bRect.w; tx++) {
        for(int ty = bRect.y; ty < bRect.y + bRect.h; ty++) {
            const auto t = board.dtile(tx, ty);
            if(t->cityId() != cid) map.setOutsideRange(tx, ty);
            const auto& ub = t->underBuilding();
            const auto ubt = ub.type();
            switch(ubt) {
            case eBuildingType::none:

            case eBuildingType::templeAphrodite:
            case eBuildingType::templeApollo:
            case eBuildingType::templeAres:
            case eBuildingType::templeArtemis:
            case eBuildingType::templeAthena:
            case eBuildingType::templeAtlas:
            case eBuildingType::templeDemeter:
            case eBuildingType::templeDionysus:
            case eBuildingType::templeHades:
            case eBuildingType::templeHephaestus:
            case eBuildingType::templeHera:
            case eBuildingType::templeHermes:
            case eBuildingType::templePoseidon:
            case eBuildingType::templeZeus:

            case eBuildingType::modestPyramid: // 3x3
            case eBuildingType::pyramid: // 5x5
            case eBuildingType::greatPyramid: // 7x7
            case eBuildingType::majesticPyramid: // 9x9

            case eBuildingType::smallMonumentToTheSky: // 5x5
            case eBuildingType::monumentToTheSky: // 6x6
            case eBuildingType::grandMonumentToTheSky: // 8x8

            case eBuildingType::minorShrineAphrodite: // 3x3
            case eBuildingType::minorShrineApollo:
            case eBuildingType::minorShrineAres:
            case eBuildingType::minorShrineArtemis:
            case eBuildingType::minorShrineAthena:
            case eBuildingType::minorShrineAtlas:
            case eBuildingType::minorShrineDemeter:
            case eBuildingType::minorShrineDionysus:
            case eBuildingType::minorShrineHades:
            case eBuildingType::minorShrineHephaestus:
            case eBuildingType::minorShrineHera:
            case eBuildingType::minorShrineHermes:
            case eBuildingType::minorShrinePoseidon:
            case eBuildingType::minorShrineZeus:

            case eBuildingType::shrineAphrodite: // 6x6
            case eBuildingType::shrineApollo:
            case eBuildingType::shrineAres:
            case eBuildingType::shrineArtemis:
            case eBuildingType::shrineAthena:
            case eBuildingType::shrineAtlas:
            case eBuildingType::shrineDemeter:
            case eBuildingType::shrineDionysus:
            case eBuildingType::shrineHades:
            case eBuildingType::shrineHephaestus:
            case eBuildingType::shrineHera:
            case eBuildingType::shrineHermes:
            case eBuildingType::shrinePoseidon:
            case eBuildingType::shrineZeus:

            case eBuildingType::majorShrineAphrodite: // 8x8
            case eBuildingType::majorShrineApollo:
            case eBuildingType::majorShrineAres:
            case eBuildingType::majorShrineArtemis:
            case eBuildingType::majorShrineAthena:
            case eBuildingType::majorShrineAtlas:
            case eBuildingType::majorShrineDemeter:
            case eBuildingType::majorShrineDionysus:
            case eBuildingType::majorShrineHades:
            case eBuildingType::majorShrineHephaestus:
            case eBuildingType::majorShrineHera:
            case eBuildingType::majorShrineHermes:
            case eBuildingType::majorShrinePoseidon:
            case eBuildingType::majorShrineZeus:

            case eBuildingType::pyramidOfThePantheon: // 11x9
            case eBuildingType::altarOfOlympus: // 8x8
            case eBuildingType::templeOfOlympus: // 8x8
            case eBuildingType::observatoryKosmika: // 9x9
            case eBuildingType::museumAtlantika: // 8x8
                continue;
            default:
                break;
            }
            const auto& rect = ub.tileRect();
            const int ttx = t->x();
            const int tty = t->y();
            if(ttx != rect.x || tty != rect.y) continue;
            if(ubt == eBuildingType::avenue) {
                addAvenueHeat(t, [&map](eThreadTile* const tile, const int value) {
                    map.addHeat(tile->dx(), tile->dy(), value);
                });
                continue;
            }
            const auto a = heatGetter(ubt);
            map.addHeat(a, rect);
        }
    }

    const auto& fms = board.finishedMonuments();
    for(const auto& m : fms) {
        const auto ubt = m.type();
        const auto& rect = m.tileRect();
        const auto a = heatGetter(ubt);
        map.addHeat(a, rect);
    }
}

void eHeatMapTask::run(eThreadBoard& board) {
    sRun(board, cid(), mBRect, mHeatGetter, mMap);
}

void eHeatMapTask::finish() {
    if(mFinish) mFinish(mMap);
}
