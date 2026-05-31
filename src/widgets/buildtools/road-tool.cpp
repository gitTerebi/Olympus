#include "road-tool.h"

#include <algorithm>
#include <cmath>

#include "engine/game-board.h"
#include "engine/etile.h"

namespace LShapeTool {

std::vector<eTile*> tiles(GameBoard* const board,
                          const int ptx, const int pty,
                          const int htx, const int hty,
                          const int firstAxis)
{
    std::vector<eTile*> result;
    if(!board) return result;
    const auto p0 = board->tile(ptx, pty);
    if(!p0) return result;
    result.push_back(p0);
    if(ptx == htx && pty == hty) return result;

    const int dx = htx - ptx;
    const int dy = hty - pty;
    const int adx = std::abs(dx);
    const int ady = std::abs(dy);
    const int sx = dx > 0 ? 1 : -1;
    const int sy = dy > 0 ? 1 : -1;
    const bool xFirst = (firstAxis == 1) ? true :
                        (firstAxis == 2) ? false :
                        (adx >= ady);

    int cx = ptx;
    int cy = pty;
    auto emit = [&]() {
        const auto t = board->tile(cx, cy);
        if(t) result.push_back(t);
    };

    if(xFirst) {
        for(int i = 0; i < adx; ++i) { cx += sx; emit(); }
        for(int i = 0; i < ady; ++i) { cy += sy; emit(); }
    } else {
        for(int i = 0; i < ady; ++i) { cy += sy; emit(); }
        for(int i = 0; i < adx; ++i) { cx += sx; emit(); }
    }
    return result;
}


} // namespace LShapeTool

void eRoadTool::noteDrag(const int pressedTX, const int pressedTY,
                         const int hoverTX, const int hoverTY) {
    if(mFirstAxis != 0) return;
    const int dx = hoverTX - pressedTX;
    const int dy = hoverTY - pressedTY;
    if(dx == 0 && dy == 0) return;
    mFirstAxis = (std::abs(dx) >= std::abs(dy)) ? 1 : 2;
}

std::vector<eTile*> eRoadTool::lShapeTiles(GameBoard* const board,
                                           const int ptx, const int pty,
                                           const int htx, const int hty) const
{
    return LShapeTool::tiles(board, ptx, pty, htx, hty, mFirstAxis);
}

std::vector<eTile*> eRoadTool::tilesHoverToPress(GameBoard* const board,
                                                 const int ptx, const int pty,
                                                 const int htx, const int hty) const
{
    auto t = lShapeTiles(board, ptx, pty, htx, hty);
    std::reverse(t.begin(), t.end());
    return t;
}
