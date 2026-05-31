#include "road-tool.h"

#include <algorithm>
#include <cmath>

#include "engine/game-board.h"
#include "engine/etile.h"

void eRoadTool::noteDrag(const int pressedTX, const int pressedTY,
                         const int hoverTX, const int hoverTY) {
    if(mFirstAxis != 0) return; // already latched this drag
    const int dx = hoverTX - pressedTX;
    const int dy = hoverTY - pressedTY;
    if(dx == 0 && dy == 0) return; // not moved off the press tile yet
    mFirstAxis = (std::abs(dx) >= std::abs(dy)) ? 1 : 2;
}

std::vector<eTile*> eRoadTool::lShapeTiles(GameBoard* const board,
                                           const int ptx, const int pty,
                                           const int htx, const int hty) const
{
    std::vector<eTile*> tiles;
    if(!board) return tiles;
    const auto p0 = board->tile(ptx, pty);
    if(!p0) return tiles;
    tiles.push_back(p0);
    if(ptx == htx && pty == hty) return tiles;

    // Roads cannot be diagonal. L-shape anchored at press: walk the fixed
    // axis to hover, then the perpendicular axis to hover. The fixed axis is
    // whichever the drag moved first (latched), so the corner stays put and
    // the L extends rather than flipping. Falls back to the larger delta when
    // unlatched (e.g. programmatic call).
    const int dx = htx - ptx;
    const int dy = hty - pty;
    const int adx = std::abs(dx);
    const int ady = std::abs(dy);
    const int sx = dx > 0 ? 1 : -1;
    const int sy = dy > 0 ? 1 : -1;
    const bool xFirst = (mFirstAxis == 1) ? true :
                        (mFirstAxis == 2) ? false :
                        (adx >= ady);

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

std::vector<eTile*> eRoadTool::tilesHoverToPress(GameBoard* const board,
                                                 const int ptx, const int pty,
                                                 const int htx, const int hty) const
{
    auto tiles = lShapeTiles(board, ptx, pty, htx, hty);
    std::reverse(tiles.begin(), tiles.end());
    return tiles;
}
