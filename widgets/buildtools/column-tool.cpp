#include "column-tool.h"

#include "engine/e-game-board.h"
#include "engine/epathfinder.h"
#include "engine/etile.h"

namespace ColumnTool {

bool path(GameBoard* const board,
          const int ptx, const int pty,
          const int htx, const int hty,
          std::vector<eOrientation>& out)
{
    if(!board) return false;
    ePathFinder p([](eTileBase* const t) {
        const auto terr = t->terrain();
        const bool tr = static_cast<bool>(eTerrain::buildable & terr);
        if(!tr) return false;
        if(t->isElevationTile()) return false;
        const auto bt = t->underBuildingType();
        const bool r = bt == eBuildingType::doricColumn ||
                       bt == eBuildingType::ionicColumn ||
                       bt == eBuildingType::corinthianColumn ||
                       bt == eBuildingType::none;
        return r;
    }, [&](eTileBase* const t) {
        return t->x() == ptx && t->y() == pty;
    });
    const auto startTile = board->tile(htx, hty);
    const int w = board->width();
    const int h = board->height();
    if(!p.findPath({0, 0, w, h}, startTile, 100, true, w, h)) return false;
    return p.extractPath(out);
}

} // namespace ColumnTool
