#ifndef ETOPBARWIDGET_H
#define ETOPBARWIDGET_H

#include "eframedwidget.h"
#include "elabel.h"

class GameBoard;
class GameWidget;

class eTopWidget : public eWidget
{
public:
    using eWidget::eWidget;

    void initialize(const std::shared_ptr<Texture> &icon,
                    const std::string &text)
    {
        setPadding(0);
        mIcon = new eScaledTextureLabel(window());
        mIcon->setPadding(0);
        mIcon->setTextureDrawScale(topSidebarTextureScale());
        mIcon->setTexture(icon);
        mIcon->fitContent();
        const int iconShrink = resolution().uiScale() == eUIScale::tiny ? 1 : 2;
        mIcon->setDrawHeight(mIcon->height() - iconShrink);
        mIcon->setDrawOffset(0, -1);
        mText = new eLabel(window());
        mText->setX(1.5 * mIcon->width());
        mText->setPadding(0);
        mText->setFontSizeS();

        addWidget(mIcon);
        addWidget(mText);

        setText(text);

        mIcon->align(Alignment::vcenter);
        mText->align(Alignment::vcenter);
    }

    bool setText(const std::string &text)
    {
        if(mText && mText->text() == text) return false;
        mText->setText(text);
        mText->fitContent();
        fitContent();
        return true;
    }

    bool setIconColor(const Uint8 r, const Uint8 g, const Uint8 b)
    {
        if(!mIcon) return false;
        if(mIconR == r && mIconG == g && mIconB == b) return false;
        mIconR = r;
        mIconG = g;
        mIconB = b;
        mIcon->setTextureColorMod(r, g, b);
        return true;
    }

private:
    eScaledTextureLabel *mIcon = nullptr;
    eLabel *mText = nullptr;
    Uint8 mIconR = 255;
    Uint8 mIconG = 255;
    Uint8 mIconB = 255;
};

class eTopBarWidget : public eWidget
{
public:
    using eWidget::eWidget;

    void initialize();
    void setBoard(GameBoard *const board);
    void setGameWidget(GameWidget *const gw);

    void paintEvent(ePainter &p);

private:
    void layoutContent();
    eWidget* createTopBarSlot(eWidget* content);
    void centerSlotContent(eWidget* slot);

    GameBoard *mBoard = nullptr;
    GameWidget *mGW = nullptr;
    eWidget *mContentWidget = nullptr;
    eWidget *mCitySlot = nullptr;
    eWidget *mDrachmasSlot = nullptr;
    eWidget *mPopulationSlot = nullptr;
    eWidget *mUnemployedSlot = nullptr;
    eWidget *mDateSlot = nullptr;
    eLabel *mCityLabel = nullptr;
    eTopWidget *mDrachmasWidget = nullptr;
    eTopWidget *mPopulationWidget = nullptr;
    eTopWidget *mUnemployedWidget = nullptr;
    eLabel *mDateLabel = nullptr;
    int mTime = 0;
};

#endif // ETOPBARWIDGET_H
