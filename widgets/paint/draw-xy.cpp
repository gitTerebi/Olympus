#include "widgets/game-widget.h"

#include "etilehelper.h"
#include "engine/game-board.h"

void GameWidget::drawXY(int tx, int ty,
                         double &rx, double &ry,
                         const int wSpan, const int hSpan,
                         const int a)
{
    if (mBoard)
    {
        const auto dir = mBoard->direction();
        if (dir != eWorldDirection::N)
        {
            const int width = mBoard->width();
            const int height = mBoard->height();
            eTileHelper::tileIdToRotatedTileId(tx, ty, tx, ty,
                                               dir, width, height);
        }
    }

    rx = tx + 0.5;
    ry = ty + 1.5;

    if (wSpan == 2 && hSpan == 2)
    {
        rx += 0.5;
        ry += 0.5;
    }
    else if (wSpan == 3 && hSpan == 3)
    {
        rx += 0.0;
        ry += 2.0;
    }
    else if (wSpan == 4 && hSpan == 4)
    {
        rx += 0.5;
        ry += 2.5;
    }
    else if (wSpan == 5 && hSpan == 5)
    {
        rx += 0.0;
        ry += 4.0;
    }
    else if (wSpan == 6 && hSpan == 6)
    {
        rx += 0.5;
        ry += 5.5;
    }
    rx -= a;
    ry -= a;
}
