#include "widgets/game-widget.h"

#include "etilehelper.h"
#include "etexturecollection.h"
#include "buildings/ebuilding.h"

stdsptr<eTexture> GameWidget::getBasementTexture(
    const int viewTileX, const int viewTileY,
    eBuilding *const building,
    const TerrainTextures &trrTexs,
    const eWorldDirection dir,
    const int boardWidth,
    const int boardHeight)
{
    auto tileRect = building->tileRect();
    tileRect = eTileHelper::toRotatedRect(tileRect, dir, boardWidth, boardHeight);
    const int right = tileRect.x + tileRect.w - 1;
    const int bottom = tileRect.y + tileRect.h - 1;
    int id = 0;
    if (tileRect.w == 1 && tileRect.h == 1)
    {
        id = 0;
    }
    else if (viewTileX == tileRect.x)
    {
        if (viewTileY == tileRect.y)
        {
            id = 2;
        }
        else if (viewTileY == bottom)
        {
            id = 8;
        }
        else
        {
            id = 9;
        }
    }
    else if (viewTileX == right)
    {
        if (viewTileY == tileRect.y)
        {
            id = 4;
        }
        else if (viewTileY == bottom)
        {
            id = 6;
        }
        else
        {
            id = 5;
        }
    }
    else if (viewTileY == tileRect.y)
    {
        id = 3;
    }
    else if (viewTileY == bottom)
    {
        id = 7;
    }
    else
    {
        id = 1;
    }
    const eTextureCollection *coll = nullptr;
    const auto type = building->type();
    if (type == eBuildingType::commonHouse ||
        type == eBuildingType::eliteHousing)
    {
        coll = &trrTexs.fBuildingBase3;
    }
    else
    {
        coll = &trrTexs.fBuildingBase2;
    }
    return coll->getTexture(id);
}
