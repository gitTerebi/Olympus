#include "widgets/game-widget.h"

#include "etilehelper.h"
#include "engine/game-board.h"

void GameWidget::drawXY(int worldTileX, int worldTileY,
                         double &drawX, double &drawY,
                         const int tileSpanW, const int tileSpanH,
                         const int altitude)
{
    int viewTileX = worldTileX;
    int viewTileY = worldTileY;
    if (mBoard)
    {
        const auto dir = mBoard->direction();
        if (dir != eWorldDirection::N)
        {
            const int boardWidth = mBoard->width();
            const int boardHeight = mBoard->height();
            eTileHelper::tileIdToRotatedTileId(worldTileX, worldTileY,
                                               viewTileX, viewTileY,
                                               dir, boardWidth, boardHeight);
        }
    }

    drawX = viewTileX + 0.5;
    drawY = viewTileY + 1.5;

    if (tileSpanW == 2 && tileSpanH == 2)
    {
        drawX += 0.5;
        drawY += 0.5;
    }
    else if (tileSpanW == 3 && tileSpanH == 3)
    {
        drawX += 0.0;
        drawY += 2.0;
    }
    else if (tileSpanW == 4 && tileSpanH == 4)
    {
        drawX += 0.5;
        drawY += 2.5;
    }
    else if (tileSpanW == 5 && tileSpanH == 5)
    {
        drawX += 0.0;
        drawY += 4.0;
    }
    else if (tileSpanW == 6 && tileSpanH == 6)
    {
        drawX += 0.5;
        drawY += 5.5;
    }
    drawX -= altitude;
    drawY -= altitude;
}
