#include "road-tool.h"

#include <algorithm>
#include <cmath>

#include "engine/e-game-board.h"
#include "engine/etile.h"

namespace RoadTool {

std::vector<eTile*> lShapeTiles(GameBoard* const board,
                                const int ptx, const int pty,
                                const int htx, const int hty)
{
    std::vector<eTile*> tiles;
    if(!board) return tiles;
    const auto p0 = board->tile(ptx, pty);
    if(!p0) return tiles;
    tiles.push_back(p0);
    if(ptx == htx && pty == hty) return tiles;

    // Roads cannot be diagonal. L-shape anchored at press: walk dominant
    // tile-coord axis to hover, then perpendicular axis to hover.
    const int dx = htx - ptx;
    const int dy = hty - pty;
    const int adx = std::abs(dx);
    const int ady = std::abs(dy);
    const int sx = dx > 0 ? 1 : -1;
    const int sy = dy > 0 ? 1 : -1;
    const bool xFirst = adx >= ady;

    int cx = ptx;
    int cy = pty;
    auto emit = [&](){
        const auto t = board->tile(cx, cy);
        if(t) tiles.push_back(t);
    };

    if(xFirst) {
        for(int i = 0; i < adx; ++i) { cx += sx; emit(); }
        for(int i = 0; i < ady; ++i) { cy += sy; emit(); }
    } else {
        for(int i = 0; i < ady; ++i) { cy += sy; emit(); }
        for(int i = 0; i < adx; ++i) { cx += sx; emit(); }
    }
    return tiles;
}

std::vector<eTile*> tilesHoverToPress(GameBoard* const board,
                                      const int ptx, const int pty,
                                      const int htx, const int hty)
{
    auto tiles = lShapeTiles(board, ptx, pty, htx, hty);
    std::reverse(tiles.begin(), tiles.end());
    return tiles;
}

} // namespace RoadTool
