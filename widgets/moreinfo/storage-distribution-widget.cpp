#include "storage-distribution-widget.h"

#include "widgets/elabel.h"
#include "widgets/emicrobutton.h"
#include "widgets/escrollbar.h"
#include "widgets/elayouthelpers.h"
#include "elanguage.h"
#include "emainwindow.h"
#include "engine/e-game-board.h"

StorageDistributionWidget::StorageDistributionWidget(
    eMainWindow *const window,
    eMainWidget *const mw) : eInfoWidget(window, mw, false, false) {}

void StorageDistributionWidget::initialize(
    eGameBoard &board, const eCityId cid)
{
    const int panelW = std::round(window()->width() * 0.75);
    resize(panelW, height());

    eInfoWidget::initialize("");
    align(eAlignment::center);

    const int p = padding();
    const int ww = widgetWidth();

    const int iconW   = std::round(ww * 0.08);
    const int nameW   = std::round(ww * 0.10);
    const int amtW    = std::round(ww * 0.10);
    const int btnW    = std::round(ww * 0.15);
    const int tradeW  = std::round(ww * 0.10);

    // header row
    {
        const auto amtHdr = new eLabel("Amount", window());
        amtHdr->setFontSizeXS();
        amtHdr->setNoPadding();
        amtHdr->fitContent();
        const auto sHdr = new eLabel("Stockpile", window());
        sHdr->setFontSizeXS();
        sHdr->setNoPadding();
        sHdr->fitContent();
        const auto mHdr = new eLabel("Mothball", window());
        mHdr->setFontSizeXS();
        mHdr->setNoPadding();
        mHdr->fitContent();
        const auto twHdr = new eLabel("Trading", window());
        twHdr->setFontSizeXS();
        twHdr->setNoPadding();
        twHdr->fitContent();
        const auto impHdr = new eLabel("Import", window());
        impHdr->setFontSizeXS();
        impHdr->setNoPadding();
        impHdr->fitContent();
        const auto expHdr = new eLabel("Export", window());
        expHdr->setFontSizeXS();
        expHdr->setNoPadding();
        expHdr->fitContent();
        const auto spacer = new eWidget(window());
        spacer->setNoPadding();
        const auto nameHdr = new eLabel("Name", window());
        nameHdr->setFontSizeXS();
        nameHdr->setNoPadding();
        nameHdr->fitContent();
        const auto hdr = eLayoutHelpers::flexRow(
            window(), ww, {{spacer, iconW, 0}, {nameHdr, nameW, 0}, {amtHdr, amtW, 0}, {sHdr, btnW, 0}, {mHdr, btnW, 0}, {twHdr, tradeW, 0}, {impHdr, tradeW, 0}, {expHdr, tradeW, 0}, {new eWidget(window()), 0, 1}}, {.gap = p});
        addInfoWidget(hdr);
    }

    // resource rows
    const auto all = eResourceType::allBasic;
    mTypes = eResourceTypeHelpers::extractResourceTypes(all);
    board.updateResources(cid);
    const auto &src = board.resources(cid);
    const auto city = board.boardCityWithId(cid);

    const auto res = window()->resolution();
    const auto uiScale = res.uiScale();

    std::vector<eLayoutHelpers::eFlexItem> rowItems;
    for (int i = 0; i < (int)mTypes.size(); i++)
    {
        const auto t = mTypes[i];

        const auto icon = new eLabel(window());
        icon->setTexture(eResourceTypeHelpers::icon(uiScale, t));
        icon->setNoPadding();
        icon->fitContent();

        const auto nameLabel = new eLabel(eResourceTypeHelpers::typeName(t), window());
        nameLabel->setFontSizeXS();
        nameLabel->setNoPadding();
        nameLabel->fitContent();

        const auto amountLabel = new eLabel("-", window());
        amountLabel->setFontSizeXS();
        amountLabel->setNoPadding();
        if (src)
            amountLabel->setText(std::to_string((*src)[i].second));
        amountLabel->fitContent();

        const bool stockpiled = city && city->isStockpiled(t);
        const auto stockpileBtn = new eMicroButton(window());
        stockpileBtn->setNoPadding();
        stockpileBtn->setFontSizeXS();
        if(stockpiled) { stockpileBtn->setText("stockpiling"); stockpileBtn->setDarkFontColor(); }
        else           { stockpileBtn->setText("using");       stockpileBtn->setLightFontColor(); }
        stockpileBtn->fitHeight();
        stockpileBtn->setPressAction([&board, cid, t, stockpileBtn]()
                                     {
            const auto c = board.boardCityWithId(cid);
            if(!c) return;
            if(c->isStockpiled(t)) { c->removeStockpile(t); stockpileBtn->setText("using");       stockpileBtn->setLightFontColor(); }
            else                   { c->addStockpile(t);    stockpileBtn->setText("stockpiling"); stockpileBtn->setDarkFontColor(); } });
        mStockpileButtons.push_back(stockpileBtn);

        const bool shutdown = city && city->isShutDown(t);
        const auto mothballBtn = new eMicroButton(window());
        mothballBtn->setNoPadding();
        mothballBtn->setFontSizeXS();
        if(shutdown) { mothballBtn->setText("disabled"); mothballBtn->setDarkFontColor(); }
        else         { mothballBtn->setText("working");  mothballBtn->setLightFontColor(); }
        mothballBtn->fitHeight();
        mothballBtn->setPressAction([&board, cid, t, mothballBtn]()
                                    {
            const auto c = board.boardCityWithId(cid);
            if(!c) return;
            if(c->isShutDown(t)) { c->removeShutDown(t); mothballBtn->setText("working");  mothballBtn->setLightFontColor(); }
            else                 { c->addShutDown(t);    mothballBtn->setText("disabled"); mothballBtn->setDarkFontColor(); } });
        mMothballButtons.push_back(mothballBtn);

        const auto twLabel = new eLabel("-", window());
        twLabel->setFontSizeXS();
        twLabel->setNoPadding();
        twLabel->fitContent();

        const auto impLabel = new eLabel(city ? std::to_string(city->totalImported(t)) : "-", window());
        impLabel->setFontSizeXS();
        impLabel->setNoPadding();
        impLabel->fitContent();

        const auto expLabel = new eLabel(city ? std::to_string(city->totalExported(t)) : "-", window());
        expLabel->setFontSizeXS();
        expLabel->setNoPadding();
        expLabel->fitContent();

        const auto row = eLayoutHelpers::flexRow(
            window(), ww,
            {{icon, iconW, 0}, {nameLabel, nameW, 0}, {amountLabel, amtW, 0}, {stockpileBtn, btnW, 0}, {mothballBtn, btnW, 0}, {twLabel, tradeW, 0}, {impLabel, tradeW, 0}, {expLabel, tradeW, 0}, {new eWidget(window()), 0, 1}},
            {.gap = p, .align = eLayoutHelpers::eAlign::center});

        rowItems.push_back({row, 0, 0});
    }

    const int maxH = std::round(window()->height() * 0.5);
    const auto scrollArea = eLayoutHelpers::flexCol(window(), 0, rowItems, {.gap = p});
    const int scrollH = std::min(scrollArea->height(), maxH);

    // probe scrollbar width before sizing viewport
    const auto sb = new eScrollBar(window());
    sb->initialize(scrollH);
    const int sbW = sb->width();

    const auto viewport = new eScrollViewport(window());
    viewport->setNoPadding();
    viewport->resize(ww - sbW, scrollH);
    scrollArea->setWidth(ww - sbW);
    viewport->setPage(scrollArea);

    sb->setViewport(viewport);
    sb->move(ww - sbW, 0);

    const auto container = new eWidget(window());
    container->setNoPadding();
    container->resize(ww, scrollH);
    container->addWidget(viewport);
    container->addWidget(sb);

    addInfoWidget(container, scrollH);

    setCloseAction([this]()
                   { eModal::close(); });
}
