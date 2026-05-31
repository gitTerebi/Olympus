#include "widgets/game-widget.h"

#include "widgets/eflatbutton.h"
#include "estringhelpers.h"
#include "characters/gods/egod.h"
#include "characters/monsters/emonster.h"
#include "engine/eresourcetype.h"
#include "engine/game-board.h"

void GameWidget::createToastWidget(eToast &toast)
{
    eEventData edCopy = toast.fEd;
    const auto msg = toast.fMsg;
    eFlatButton* tw = new eFlatButton(window());
    tw->setNoPadding();
    tw->setFontSizeXS();
    auto title = msg.fTitle;
    if (const auto &c = toast.fEd.fCity)
        eStringHelpers::replaceAll(title, "[city_name]", c->name());
    if (const auto &c = toast.fEd.fRivalCity)
        eStringHelpers::replaceAll(title, "[rival_city_name]", c->name());
    eStringHelpers::replaceAll(title, "[item]",
                               eResourceTypeHelpers::typeLongName(toast.fEd.fResourceType));
    eStringHelpers::replaceAll(title, "[itemshort]",
                               eResourceTypeHelpers::typeName(toast.fEd.fResourceType));
    eStringHelpers::replaceAll(title, "[god]", eGod::sGodName(toast.fEd.fGod));
    eStringHelpers::replaceAll(title, "[monster]", eMonster::sMonsterName(toast.fEd.fMonster));
    if (title.length() > 40)
        title = title.substr(0, 37) + "...";
    tw->setText(title);
    tw->fitContent();
    tw->setPressAction([this, edCopy, msg, tw]() mutable
    {
        for (int i = 0; i < int(mToasts.size()); i++)
        {
            if (mToasts[i].fWid == tw)
            {
                mToasts.erase(mToasts.begin() + i);
                break;
            }
        }
        tw->deleteLater();
        updateToastPositions();
        showMessage(edCopy, msg, false, true, false);
    });
    const int p = tw->padding();
    tw->resize(tw->width() + 2 * p, tw->height() + 2 * p);
    const int vw = width() - mGm->width();
    tw->setX((vw - tw->width()) / 2);
    toast.fWid = tw;
    addWidget(tw);
}

void GameWidget::showToast(eEventData &ed, const eMessage &msg)
{
    eToast pendingToast;
    pendingToast.fEd = ed;
    pendingToast.fMsg = msg;
    pendingToast.fDate = mBoard->date();
    pendingToast.fExpireFrame = 0;
    pendingToast.fQueued = false;
    mPendingToasts.push_back(pendingToast);
}

void GameWidget::updateToastPositions()
{
    const int p = padding();
    int y;
    if (mPausedLabel)
    {
        y = mPausedLabel->y() + mPausedLabel->height() + 2 * p;
    }
    else
    {
        y = 5 * p;
    }
    if (mSpeedLabel && mSpeedLabel->visible())
    {
        y += mSpeedLabel->height() + 2 * p;
    }
    for (const auto &tip : mTips)
    {
        y += tip.fWid->height() + 2 * p;
    }
    for (const auto &toast : mToasts)
    {
        const auto w = toast.fWid;
        w->setY(y);
        y += w->height() + 2 * p;
    }
}
