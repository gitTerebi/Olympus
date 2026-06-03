#include "draw-column.h"

#include "widgets/etilepainter.h"
#include "etexturecollection.h"

void drawColumn(eTilePainter& tp, const int n,
                const double drawX, const double drawY,
                const eTextureCollection& coll)
{
    double y = 0;
    const auto top = coll.getTexture(0);
    const auto mid = coll.getTexture(1);
    const auto btm = coll.getTexture(2);

    tp.drawTexture(drawX + 1 - y, drawY - y, btm,
                   eAlignment::hcenter | eAlignment::top);
    y += 0.75;
    for (int i = 0; i < n; i++)
    {
        tp.drawTexture(drawX + 1 - y, drawY - y, mid,
                       eAlignment::hcenter | eAlignment::top);
        y += 0.33;
    }
    tp.drawTexture(drawX + 1 - y, drawY - y, top,
                   eAlignment::hcenter | eAlignment::top);
}
