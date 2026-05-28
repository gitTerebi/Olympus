#include "widgets/gamebuild/ecommonhousingbuild.h"

#include "engine/e-game-board.h"
#include "engine/ecityid.h"
#include "engine/edifficulty.h"
#include "buildings/allbuildings.h"

#include <algorithm>

std::vector<eCommonHousingTileRect> commonHousingBuildRects(
        GameBoard* board, eCityId cid, ePlayerId pid, bool editorMode,
        int pressedTX, int pressedTY, int hoverTX, int hoverTY) {
    std::vector<eCommonHousingTileRect> rects;
    if(!board) return rects;

    const int dx = hoverTX < pressedTX ? -2 : 2;
    const int dy = hoverTY < pressedTY ? -2 : 2;
    const int startY = pressedTY - 1;
    const int endY = hoverTY < pressedTY ? hoverTY - 1 : hoverTY;

    for(int x = pressedTX; dx > 0 ? x <= hoverTX : x >= hoverTX; x += dx) {
        for(int y = startY; dy > 0 ? y <= endY : y >= endY; y += dy) {
            const bool cb = board->canBuildBase(x, x + 2, y, y + 2, editorMode, cid, pid);
            if(!cb) continue;
            rects.push_back({x, y, 2, 2});
        }
    }
    return rects;
}

eCommonHousingTileRect commonHousingBuildBounds(
        const std::vector<eCommonHousingTileRect>& rects) {
    if(rects.empty()) return {0, 0, 0, 0};

    int minX = rects.front().fX;
    int minY = rects.front().fY;
    int maxX = rects.front().fX + rects.front().fW;
    int maxY = rects.front().fY + rects.front().fH;
    for(const auto& rect : rects) {
        minX = std::min(minX, rect.fX);
        minY = std::min(minY, rect.fY);
        maxX = std::max(maxX, rect.fX + rect.fW);
        maxY = std::max(maxY, rect.fY + rect.fH);
    }
    return {minX, minY, maxX - minX, maxY - minY};
}

bool buildCommonHousing(GameBoard* board, eCityId cid, ePlayerId pid, bool editorMode,
                        int pressedTX, int pressedTY, int hoverTX, int hoverTY, eCityId viewedCityId) {
    const auto rects = commonHousingBuildRects(
                board, cid, pid, editorMode,
                pressedTX, pressedTY, hoverTX, hoverTY);
    int totalCost = 0;
    const auto diff = board->difficulty(pid);
    const int costPerHouse = eDifficultyHelpers::buildingCost(diff, eBuildingType::commonHouse);
    for(const auto& rect : rects) {
        (void)rect;
        totalCost += costPerHouse;
    }
    bool r = false;
    if(totalCost > 0) {
        const auto bounds = commonHousingBuildBounds(rects);
        board->game_undo_start_build(eBuildingType::commonHouse);
        board->snapshotTiles(bounds.fX, bounds.fY, bounds.fW, bounds.fH);
        for(const auto& rect : rects) {
            const auto t = board->tile(rect.fX, rect.fY);
            if(!t) continue;
            r = board->build(t->x(), t->y() + 1, 2, 2, cid, pid, editorMode,
                  [board, viewedCityId]() { return e::make_shared<eSmallHouse>(*board, viewedCityId); }) || r;
        }
        board->game_undo_finish_build();
    }
    return r;
}
