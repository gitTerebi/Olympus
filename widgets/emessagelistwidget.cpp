#include "emessagelistwidget.h"

#include "escrollbar.h"
#include "eflatbutton.h"
#include "elabel.h"
#include "eframedlabel.h"
#include "estringhelpers.h"
#include "engine/eworldcity.h"
#include "characters/gods/egod.h"
#include "characters/monsters/emonster.h"
#include "engine/eresourcetype.h"
#include "eokbutton.h"
#include "emainwindow.h"

void eMessageListWidget::initialize(const eOpenMessage &openMsg, const eAction &closeAction)
{
    mOpenMsg = openMsg;
    mOnClose = closeAction;
    setType(eFrameType::message);

    const int p = padding();

    const auto title = new eLabel(window());
    title->setFontSizeS();
    title->setPaddingXS();
    title->setText("Messages");
    title->setDarkFontColor();
    title->fitContent();
    title->setX((width() - title->width()) / 2);
    title->setY(p);
    addWidget(title);

    const auto cancel = new eOkButton(window());
    cancel->fitContent();
    cancel->setPaddingXS();
    cancel->setPressAction([this]()
                           {
        hide();
        if(mOnClose) mOnClose(); });
    addWidget(cancel);
    cancel->setX(width() - cancel->width() - padding() - 20);
    cancel->setY(height() - cancel->height() - padding());

    const int cw = width() - 4 * p;
    const int ch = cancel->y() - 2 * p - (title->y() + title->height()) - p;

    const auto sidebar = new eScrollBar(window());
    sidebar->initialize(ch);
    mSidebar = sidebar;
    const int vpW = cw - sidebar->width() - p;
    mVpWidth = vpW;

    // Blue panel background (same as building info)
    const auto bg = new eFramedWidget(window());
    bg->setType(eFrameType::inner);
    bg->resize(cw - sidebar->width() - p, ch);
    bg->move(2 * p, title->y() + title->height() + p);
    addWidget(bg);

    const auto tp = mTp;
    mViewport = new eScrollViewport(window());
    mViewport->setNoPadding();
    mViewport->resize(vpW - 2 * tp, ch - 2 * tp);
    mViewport->move(tp, tp);
    bg->addWidget(mViewport);

    sidebar->move(2 * p + vpW + p, title->y() + title->height() + p);
    addWidget(sidebar);
    sidebar->setViewport(mViewport);

    mContentArea = new eWidget(window());
    mContentArea->setNoPadding();
    mViewport->setPage(mContentArea);
}

void eMessageListWidget::markAllRead()
{
    for (int i = 0; i < static_cast<int>(mMessages.size()); i++)
    {
        auto &lm = mMessages[i];
        if(!lm.fRead && mReadChanged) mReadChanged(i);
        lm.fRead = true;
    }
    mUnreadCount = 0;
    notifyUnread();
    rebuildList();
}

void eMessageListWidget::notifyUnread()
{
    if (mUnreadChanged)
        mUnreadChanged(unreadCount());
}

void eMessageListWidget::addMessage(const eEventData &ed, const eMessage &msg, const eDate &date)
{
    eLoggedMessage lm;
    lm.fEd = ed;
    lm.fEd.fCA0 = nullptr;
    lm.fEd.fA0 = nullptr;
    lm.fEd.fCCA0.clear();
    lm.fEd.fA1 = nullptr;
    lm.fEd.fA2 = nullptr;
    lm.fEd.fType = eMessageEventType::common;
    lm.fMsg = msg;
    lm.fRead = false;
    lm.fDate = date;

    // Format title
    std::string title = msg.fTitle;
    if (const auto &c = ed.fCity)
        eStringHelpers::replaceAll(title, "[city_name]", c->name());
    if (const auto &c = ed.fRivalCity)
        eStringHelpers::replaceAll(title, "[rival_city_name]", c->name());
    eStringHelpers::replaceAll(title, "[item]",
                               eResourceTypeHelpers::typeLongName(ed.fResourceType));
    eStringHelpers::replaceAll(title, "[itemshort]",
                               eResourceTypeHelpers::typeName(ed.fResourceType));
    eStringHelpers::replaceAll(title, "[god]", eGod::sGodName(ed.fGod));
    eStringHelpers::replaceAll(title, "[monster]", eMonster::sMonsterName(ed.fMonster));
    lm.fFormattedTitle = title;

    // Format date
    const int year = date.year();
    const std::string yearStr = year < 0 ? std::to_string(-year) + " BC" : std::to_string(year);
    lm.fDateStr = eMonthHelper::shortName(date.month()) + " " + yearStr;

    mMessages.push_back(lm);
    if (mMessages.size() > 50)
    {
        if (mMessages.front().fRead == false)
            mUnreadCount--;
        mMessages.erase(mMessages.begin());
    }
    mUnreadCount++;
    notifyUnread();
    rebuildList();
}

void eMessageListWidget::addSavedMessage(const eEventData &ed,
                                         const eMessage &msg,
                                         const eDate &date,
                                         const bool read)
{
    eLoggedMessage lm;
    lm.fEd = ed;
    lm.fMsg = msg;
    lm.fRead = read;
    lm.fDate = date;

    lm.fFormattedTitle = msg.fTitle;

    const int year = date.year();
    const std::string yearStr = year < 0 ? std::to_string(-year) + " BC" : std::to_string(year);
    lm.fDateStr = eMonthHelper::shortName(date.month()) + " " + yearStr;

    mMessages.push_back(lm);
    if (mMessages.size() > 50)
    {
        mMessages.erase(mMessages.begin());
    }
    if(!read) mUnreadCount++;
    notifyUnread();
    rebuildList();
}

void eMessageListWidget::rebuildList()
{
    if (!mViewport)
        return;

    const auto children = mContentArea->children();
    for (const auto c : children)
    {
        c->deleteLater();
    }

    const int w = mVpWidth - 2 * mTp;
    const int pp = 0; // no padding
    int y = 8;

    for (int i = (int)mMessages.size() - 1; i >= 0; i--)
    {
        auto &lm = mMessages[i];

        auto title = lm.fFormattedTitle;

        // Date label
        const int year = lm.fDate.year();
        const std::string yearStr = year < 0 ? std::to_string(-year) + " BC" : std::to_string(year);
        auto dateStr = lm.fDateStr;
        const auto dateLabel = new eLabel(dateStr, window());
        dateLabel->setFontSizeXS();
        dateLabel->setNoPadding();
        if (lm.fRead)
            dateLabel->setFontColor(eFontColor::dark);
        else
            dateLabel->setFontColor(eFontColor::light);
        dateLabel->setWidth(w * 0.1 - 2 * pp);
        dateLabel->setHeight(20);
        dateLabel->setX(50);
        dateLabel->setY(0);

        // Title label
        const auto titleLabel = new eLabel(title, window());
        titleLabel->setFontSizeXS();
        titleLabel->setTextAlignment(eAlignment::left);
        titleLabel->setNoPadding();
        if (lm.fRead)
            titleLabel->setFontColor(eFontColor::dark);
        else
            titleLabel->setFontColor(eFontColor::light);
        titleLabel->setWidth(w * 0.9 - 2 * pp);
        titleLabel->setHeight(20);
        titleLabel->setX(dateLabel->width() * 3);
        titleLabel->setY(0);

        // Row widget
        const int index = i;
        const auto row = new eMessageListRow(window(), dateLabel, titleLabel, [this, index]()
                                             {
            auto& lm = mMessages[index];
            mOpenMsg(lm.fEd, lm.fMsg);
            if(!lm.fRead) {
                lm.fRead = true;
                mUnreadCount--;
                if(mReadChanged) mReadChanged(index);
            }
            notifyUnread();
            rebuildList();
        });
        row->resize(w - 2 * pp, 20);
        row->setX(pp);
        row->setY(y);

        mContentArea->addWidget(row);

        y += 28;
    }
    mContentArea->fitContent();
    mContentArea->setWidth(w);
    mViewport->setPage(mContentArea);
}

bool eMessageListWidget::keyPressEvent(const eKeyPressEvent &e)
{
    const auto k = e.key();
    if (k == SDL_SCANCODE_ESCAPE)
    {
        close();
        return true;
    }
    return eModal::keyPressEvent(e);
}

