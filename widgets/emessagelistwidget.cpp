#include "emessagelistwidget.h"

#include "escrollwidgetcomplete.h"
#include "eflatbutton.h"
#include "elabel.h"
#include "eframedlabel.h"
#include "estringhelpers.h"
#include "engine/eworldcity.h"
#include "characters/gods/egod.h"
#include "characters/monsters/emonster.h"
#include "engine/eresourcetype.h"
#include "eokbutton.h"

void eMessageListWidget::initialize(const eOpenMessage& openMsg, const eAction& closeAction) {
    mOpenMsg = openMsg;
    mOnClose = closeAction;
    setType(eFrameType::message);

    const int p = padding();

    const auto title = new eLabel(window());
    title->setSmallFontSize();
    title->setTinyPadding();
    title->setText("Messages");
    title->setDarkFontColor();
    title->fitContent();
    title->setX((width() - title->width()) / 2);
    title->setY(p);
    addWidget(title);

    const auto cancel = new eOkButton(window());
    cancel->fitContent();
    cancel->setTinyPadding();
    cancel->setPressAction([this]() {
        hide();
        if(mOnClose) mOnClose();
    });
    addWidget(cancel);
    cancel->setX(width() - cancel->width() - padding() - 20);
    cancel->setY(height() - cancel->height() - padding());

    const int cw = width() - 4*p;
    const int ch = cancel->y() - 2*p - (title->y() + title->height()) - p;

    // Blue panel background (same as building info)
    const auto bg = new eFramedWidget(window());
    bg->setType(eFrameType::inner);
    bg->resize(cw, ch);
    bg->move(2*p, title->y() + title->height() + p);
    addWidget(bg);

    mListArea = new eWidget(window());
    mListArea->setNoPadding();
    mListArea->resize(cw, ch);
    mListArea->setY(title->y() + title->height() + p);
    mListArea->setX(2*p);
    addWidget(mListArea);

}



void eMessageListWidget::markAllRead() {
    for(auto& lm : mMessages) {
        lm.fRead = true;
    }
    mUnreadCount = 0;
    notifyUnread();
    rebuildList();
}

void eMessageListWidget::notifyUnread() {
    if(mUnreadChanged) mUnreadChanged(unreadCount());
}

void eMessageListWidget::addMessage(const eEventData& ed, const eMessage& msg, const eDate& date) {
    eLoggedMessage lm;
    lm.fEd = ed;
    lm.fMsg = msg;
    lm.fRead = false;
    lm.fDate = date;

    // Format title
    std::string title = msg.fTitle;
    if(const auto& c = ed.fCity)
        eStringHelpers::replaceAll(title, "[city_name]", c->name());
    if(const auto& c = ed.fRivalCity)
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
    if(mMessages.size() > 50) {
        if(mMessages.front().fRead == false) mUnreadCount--;
        mMessages.erase(mMessages.begin());
    }
    mUnreadCount++;
    notifyUnread();
    rebuildList();
}

void eMessageListWidget::rebuildList() {
    if(!mListArea) return;

    const auto children = mListArea->children();
    for(const auto c : children) {
        c->deleteLater();
    }

    const int w = mListArea->width();
    const int p = mListArea->padding();
    int y = 8;

    for(int i = (int)mMessages.size() - 1; i >= 0; i--) {
        auto& lm = mMessages[i];

        auto title = lm.fFormattedTitle;

        // Date label
        const int year = lm.fDate.year();
        const std::string yearStr = year < 0 ? std::to_string(-year) + " BC" : std::to_string(year);
        auto dateStr = lm.fDateStr;
        const auto dateLabel = new eLabel(dateStr, window());
        dateLabel->setVerySmallFontSize();
        dateLabel->setNoPadding();
        if(lm.fRead) 
            dateLabel->setFontColor(eFontColor::dark);
        else
            dateLabel->setFontColor(eFontColor::light);
        dateLabel->setWidth(w * 0.1 - 2*p);
        dateLabel->setHeight(20);
        dateLabel->setX(50);
        dateLabel->setY(0);

        // Title label
        const auto titleLabel = new eLabel(title, window());
        titleLabel->setVerySmallFontSize();
        titleLabel->setTextAlignment(eAlignment::left);
        titleLabel->setNoPadding();
        if(lm.fRead) 
            titleLabel->setFontColor(eFontColor::dark);
        else
            titleLabel->setFontColor(eFontColor::light);
        titleLabel->setWidth(w * 0.9 - 2*p);
        titleLabel->setHeight(20);
        titleLabel->setX(dateLabel->width() * 3);
        titleLabel->setY(0);

        // Row widget
        const auto row = new eMessageListRow(window(), dateLabel, titleLabel, [this, &lm]() { mOpenMsg(lm.fEd, lm.fMsg); if(!lm.fRead) { lm.fRead = true; mUnreadCount--; } notifyUnread(); rebuildList(); });
        row->resize(w - 2*p, 20);
        row->setX(p);
        row->setY(y);

        mListArea->addWidget(row);

        y += 28;
}
}

bool eMessageListWidget::keyPressEvent(const eKeyPressEvent& e) {
    const auto k = e.key();
    if(k == SDL_SCANCODE_ESCAPE) {
        hide();
        if(mOnClose) mOnClose();
        return true;
    }
    return eClosableDialog::keyPressEvent(e);
}

bool eMessageListWidget::mouseReleaseEvent(const eMouseEvent& e) {
    if(e.button() == eMouseButton::right) {
        hide();
        if(mOnClose) mOnClose();
        return true;
    }
    return eClosableDialog::mouseReleaseEvent(e);
}




