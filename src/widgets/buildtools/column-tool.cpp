#include "column-tool.h"

#include "road-tool.h"
#include "engine/game-board.h"
#include "engine/etile.h"

namespace ColumnTool {

bool path(GameBoard* const board,
          const int ptx, const int pty,
          const int htx, const int hty,
          const int firstAxis,
          std::vector<eOrientation>& out)
{
    if(!board) return false;
    const auto ts = LShapeTool::tiles(board, ptx, pty, htx, hty, firstAxis);
    if(ts.empty()) return false;
    out.clear();
    out.reserve(ts.size() - 1);
    for(int i = 0; i + 1 < static_cast<int>(ts.size()); ++i) {
        const auto a = ts[i];
        const auto b = ts[i + 1];
        const int dx = b->x() - a->x();
        const int dy = b->y() - a->y();
        eOrientation o;
        if(dx > 0)      o = eOrientation::bottomRight;
        else if(dx < 0) o = eOrientation::topLeft;
        else if(dy > 0) o = eOrientation::bottomLeft;
        else             o = eOrientation::topRight;
        out.push_back(!o);
    }
    return true;
}

} // namespace ColumnTool
