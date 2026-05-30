#include "widgets/game-widget.h"

#include "etilehelper.h"
#include "etexturecollection.h"
#include "buildings/ebuilding.h"

stdsptr<eTexture> GameWidget::getBasementTexture(
    const int rtx, const int rty,
    eBuilding *const d,
    const eTerrainTextures &trrTexs,
    const eWorldDirection dir,
    const int boardw,
    const int boardh)
{
    auto tr = d->tileRect();
    tr = eTileHelper::toRotatedRect(tr, dir, boardw, boardh);
    const int right = tr.x + tr.w - 1;
    const int bottom = tr.y + tr.h - 1;
    int id = 0;
    if (tr.w == 1 && tr.h == 1)
    {
        id = 0;
    }
    else if (rtx == tr.x)
    {
        if (rty == tr.y)
        {
            id = 2;
        }
        else if (rty == bottom)
        {
            id = 8;
        }
        else
        {
            id = 9;
        }
    }
    else if (rtx == right)
    {
        if (rty == tr.y)
        {
            id = 4;
        }
        else if (rty == bottom)
        {
            id = 6;
        }
        else
        {
            id = 5;
        }
    }
    else if (rty == tr.y)
    {
        id = 3;
    }
    else if (rty == bottom)
    {
        id = 7;
    }
    else
    {
        id = 1;
    }
    const eTextureCollection *coll = nullptr;
    const auto type = d->type();
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
