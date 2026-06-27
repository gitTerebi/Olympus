#include "eworldgoodswidget.h"

#include "elabel.h"
#include "ebutton.h"
#include "textures/game-textures.h"
#include "eworldgoodswidget.h"
#include "engine/game-board.h"
#include "framed-button.h"
#include "eresourcebutton.h"
#include "evaluebutton.h"
#include "vector-helpers.h"

#include "main-window.h"
#include "ecancelbutton.h"

#include "language.h"

void eTradeTypesWidget::initialize() {
    setType(eFrameType::inner);
}

void eTradeTypesWidget::setTrade(const ePlayerId pid,
                                 const std::vector<eResourceTrade>& trade) {
    const auto uiScale = resolution().uiScale();
    int iRes;
    double mult;
    iResAndMult(iRes, mult);

    const int h = mult*12;

    const int n = trade.size();
    setHeight(n*h);

    removeChildren();
    int i = 0;
    for(const auto& t : trade) {
        const auto w = new eWidget(window());
        w->setHeight(h);
        w->setX(h/2);
        w->setWidth(width() - h);
        w->setNoPadding();

        const auto icon = eResourceTypeHelpers::icon(uiScale, t.fType);
        const auto iconLabel = new eLabel(window());
        iconLabel->setNoPadding();
        iconLabel->setTexture(icon);
        iconLabel->fitContent();
        w->addWidget(iconLabel);
        iconLabel->align(Alignment::vcenter | Alignment::left);

        const auto name = eResourceTypeHelpers::typeName(t.fType);
        const auto nameLabel = new eLabel(name, window());
        nameLabel->setFontSizeXS();
        nameLabel->fitContent();
        w->addWidget(nameLabel);
        nameLabel->align(Alignment::vcenter | Alignment::left);
        nameLabel->setX(iconLabel->x() + iconLabel->width());

        const auto used = std::to_string(t.used(pid));
        const auto max = std::to_string(t.fMax);
        const auto str = used + "/" + max;
        const auto usedLabel = new eLabel(str, window());
        usedLabel->setFontSizeXS();
        usedLabel->fitContent();
        w->addWidget(usedLabel);
        usedLabel->align(Alignment::vcenter | Alignment::right);

        addWidget(w);
        w->setY(i*h);
        i++;
    }
}

void eWorldTradeWidget::initialize(const std::string& name,
                                   const int fullWidth) {
    int iRes;
    double mult;
    iResAndMult(iRes, mult);

    setWidth(fullWidth > 0 ? fullWidth : static_cast<int>(mult*75));

    mNameLabel = new eLabel(window());
    mNameLabel->setPaddingXS();
    mNameLabel->setFontSizeS();
    mNameLabel->setText(name);
    mNameLabel->fitContent();
    addWidget(mNameLabel);
    mNameLabel->align(Alignment::top | Alignment::hcenter);

    mTradeTypesWidget = new eTradeTypesWidget(window());
    mTradeTypesWidget->setY(mNameLabel->height());
    mTradeTypesWidget->initialize();
    mTradeTypesWidget->setWidth(width());
    addWidget(mTradeTypesWidget);

    setNoPadding();
    fitContent();
}

void eWorldTradeWidget::setName(const std::string &name) {
    mNameLabel->setText(name);
    mNameLabel->fitContent();
    mNameLabel->align(Alignment::top | Alignment::hcenter);
}

void eWorldTradeWidget::setTrade(const ePlayerId pid,
                                 std::vector<eResourceTrade>* const trade) {
    mTrade = trade;
    const int oh = height();
    mTradeTypesWidget->setTrade(pid, trade ? *trade : std::vector<eResourceTrade>{});
    fitContent();
    const int nh = height();
    setY(y() + oh - nh);
}

void eWorldGoodsWidget::initialize() {
    int iRes;
    double mult;
    iResAndMult(iRes, mult);
    const auto& intrfc = GameTextures::interface();
    const auto& coll = intrfc[iRes];

    mGoodsLabel = new eLabel(window());
    mGoodsLabel->setFontSizeS();
    mGoodsLabel->setPaddingXS();
    const auto goodsStr = Language::zeusText(47, 6);
    mGoodsLabel->setText(goodsStr);
    mGoodsLabel->fitContent();
    addWidget(mGoodsLabel);
    mGoodsLabel->align(Alignment::top | Alignment::hcenter);

    mOrdersButton = eButton::sCreate(coll.fWorldSmallButton, window(), this);

    const auto ordersStr = Language::zeusText(47, 7);
    const auto ordersTxt = new eLabel(ordersStr, window());
    ordersTxt->setFontSizeS();
    ordersTxt->fitContent();
    mOrdersButton->addWidget(ordersTxt);
    ordersTxt->align(Alignment::center);
    mOrdersButton->align(Alignment::hcenter);
    mOrdersButton->setY(mGoodsLabel->y() + mGoodsLabel->height());

    mBuysWidget = new eWorldTradeWidget(window());
    mBuysWidget->initialize(Language::zeusText(47, 1), width());
    addWidget(mBuysWidget);

    mSellsWidget = new eWorldTradeWidget(window());
    mSellsWidget->initialize(Language::zeusText(47, 2), width());
    addWidget(mSellsWidget);

    updateTradeY();
}

void eWorldGoodsWidget::setCity(const stdsptr<WorldCity>& c) {
    const auto buys = c ? &c->buys() : nullptr;
    const auto sells = c ? &c->sells() : nullptr;
    const bool cc = c && c->isCurrentCity();
    mBuysWidget->setTrade(mPid, buys);
    mSellsWidget->setTrade(mPid, sells);
    const bool neb = buys && !buys->empty();
    const bool nes = sells && !sells->empty();
    mBuysWidget->setVisible(neb && !cc);
    mSellsWidget->setVisible(nes && !cc);
    mGoodsLabel->setVisible(c.get() && !cc && (neb || nes));
    mOrdersButton->setVisible((neb || nes) && !cc);
    updateTradeY();

    if(c) {
        if(c->isRival()) {
            mBuysWidget->setName(Language::zeusText(47, 3));
            mSellsWidget->setName(Language::zeusText(47, 4));
        } else {
            mBuysWidget->setName(Language::zeusText(47, 1));
            mSellsWidget->setName(Language::zeusText(47, 2));
        }
    }
}

void eWorldGoodsWidget::updateTradeY() const {
    mSellsWidget->setY(height() - mSellsWidget->height());
    mBuysWidget->setY(mSellsWidget->y() - mBuysWidget->height());
}
