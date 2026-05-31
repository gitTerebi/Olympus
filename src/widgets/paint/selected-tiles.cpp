#include "widgets/game-widget.h"

#include "etilehelper.h"
#include "engine/game-board.h"

#include <algorithm>
#include <vector>

std::vector<eTile *> GameWidget::selectedTiles() const
{
    std::vector<eTile *> result;
    const int x0 = mPressedX > mHoverX ? mHoverX : mPressedX;
    const int y0 = mPressedY > mHoverY ? mHoverY : mPressedY;
    const int x1 = mPressedX > mHoverX ? mPressedX : mHoverX;
    const int y1 = mPressedY > mHoverY ? mPressedY : mHoverY;
    int t0x;
    int t0y;
    int t1x;
    int t1y;
    pixToId(x0, y0, t0x, t0y);
    pixToId(x1, y1, t1x, t1y);

    int dt0x;
    int dt0y;
    eTileHelper::tileIdToDTileId(t0x, t0y, dt0x, dt0y);
    int dt1x;
    int dt1y;
    eTileHelper::tileIdToDTileId(t1x, t1y, dt1x, dt1y);

    const int xMin = std::min(dt0x, dt1x);
    const int xMax = std::max(dt0x, dt1x);
    const int yMin = std::min(dt0y, dt1y);
    const int yMax = std::max(dt0y, dt1y);
    for (int x = xMin; x < xMax; x++)
    {
        for (int y = yMin; y < yMax; y++)
        {
            const auto tile = mBoard->dtile(x, y);
            if (!tile)
                continue;
            const auto cid = tile->cityId();
            if (cid != mViewedCityId)
                continue;
            result.push_back(tile);
        }
    }
    return result;
}
