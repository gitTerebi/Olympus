#ifndef ETOPBARWIDGET_H
#define ETOPBARWIDGET_H

#include "eframedwidget.h"
#include "elabel.h"

class eGameBoard;
class eGameWidget;
class eButton;

class eTopWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const std::shared_ptr<eTexture>& icon,
                    const std::string& text) {
        setPadding(0);
        mIcon = new eLabel(window());
        mIcon->setPadding(0);
        mIcon->setTexture(icon);
        mIcon->fitContent();
        mText = new eLabel(window());
        mText->setX(1.5*mIcon->width());
        mText->setPadding(0);
        mText->setFontSizeS();

        addWidget(mIcon);
        addWidget(mText);

        setText(text);

        mIcon->align(eAlignment::vcenter);
        mText->align(eAlignment::vcenter);
    }

    void setText(const std::string& text) {
        mText->setText(text);
        mText->fitContent();
        fitContent();
    }

    void setIconColor(const Uint8 r, const Uint8 g, const Uint8 b) {
        if(mIcon) mIcon->setTextureColorMod(r, g, b);
    }

private:
    eLabel* mIcon = nullptr;
    eLabel* mText = nullptr;
};

class eTopBarWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize();
    void setBoard(eGameBoard* const board);
    void setGameWidget(eGameWidget* const gw);

    void paintEvent(ePainter& p);
private:
    eGameBoard* mBoard = nullptr;
    eGameWidget* mGW = nullptr;
    eLabel* mCityLabel = nullptr;
    eTopWidget* mDrachmasWidget = nullptr;
    eTopWidget* mPopulationWidget = nullptr;
    eTopWidget* mUnemployedWidget = nullptr;
    eButton* mDateLabel = nullptr;
    int mTime = 0;
};

#endif // ETOPBARWIDGET_H
