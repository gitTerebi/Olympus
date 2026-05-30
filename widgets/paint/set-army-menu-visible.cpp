#include "widgets/game-widget.h"

#include "widgets/eterraineditmenu.h"
#include "widgets/eminimap.h"

void GameWidget::setArmyMenuVisible(const bool v)
{
    if (mAm->visible() == v)
        return;
    mAm->setVisible(v);
    if (v)
    {
        mGm->show();
        mTem->hide();
        const auto map = mAm->miniMap();
        map->scheduleUpdate();
    }
    else
    {
        mTem->setVisible(mTerrainEditMode);
        mGm->setVisible(!mTerrainEditMode);
    }
}
