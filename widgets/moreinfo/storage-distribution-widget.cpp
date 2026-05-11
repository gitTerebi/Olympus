#include "storage-distribution-widget.h"

#include "widgets/elabel.h"
#include "widgets/emicrobutton.h"
#include "elanguage.h"
#include "emainwindow.h"
#include "engine/e-game-board.h"

StorageDistributionWidget::StorageDistributionWidget(
        eMainWindow* const window,
        eMainWidget* const mw) :
    eInfoWidget(window, mw, false, false) {}

void StorageDistributionWidget::initialize(
        eGameBoard& board, const eCityId cid) {
    const int panelW = std::round(window()->width() * 0.75);
    resize(panelW, height());

    eInfoWidget::initialize("");
    align(eAlignment::center);

    const int p = padding();
    const int ww = widgetWidth();

    // fixed button col width — enough for "[S]" / "[M]" text
    const int btnW = std::round(ww * 0.15);
    const int iconW = std::round(ww * 0.08);
    const int amtW  = ww - iconW - 2*btnW - 3*p;

    // header row
    {
        const auto hdr = new eWidget(window());
        hdr->setNoPadding();
        int x = iconW + p;
        const std::string hdrs[3] = { "Amount", "Stockpile", "Mothball" };
        const int widths[3] = { amtW, btnW, btnW };
        for(int i = 0; i < 3; i++) {
            const auto l = new eLabel(hdrs[i], window());
            l->setFontSizeXS();
            l->setNoPadding();
            l->fitContent();
            l->setX(x);
            hdr->addWidget(l);
            x += widths[i] + p;
        }
        hdr->fitContent();
        addInfoWidget(hdr);
    }

    // resource rows
    const auto all = eResourceType::allBasic;
    mTypes = eResourceTypeHelpers::extractResourceTypes(all);
    board.updateResources(cid);
    const auto& src = board.resources(cid);
    const auto city = board.boardCityWithId(cid);

    const auto res = window()->resolution();
    const auto uiScale = res.uiScale();

    for(int i = 0; i < (int)mTypes.size(); i++) {
        const auto t = mTypes[i];
        const auto row = new eWidget(window());
        row->setNoPadding();
        int x = 0;

        // icon
        const auto icon = eResourceTypeHelpers::icon(uiScale, t);
        const auto ic = new eLabel(window());
        ic->setTexture(icon);
        ic->setNoPadding();
        ic->fitContent();
        ic->setX(x);
        row->addWidget(ic);
        x += iconW + p;

        // amount
        const auto l = new eLabel("-", window());
        l->setFontSizeXS();
        l->setNoPadding();
        if(src) l->setText(std::to_string((*src)[i].second));
        l->fitContent();
        l->setX(x);
        row->addWidget(l);
        x += amtW + p;

        // stockpile button
        const auto sb = new eMicroButton(window());
        sb->setNoPadding();
        sb->setFontSizeXS();
        sb->setText(city && city->isStockpiled(t) ? "[S]" : "S");
        sb->setWidth(btnW);
        sb->fitHeight();
        sb->setX(x);
        sb->setPressAction([&board, cid, t, sb]() {
            const auto c = board.boardCityWithId(cid);
            if(!c) return;
            if(c->isStockpiled(t)) { c->removeStockpile(t); sb->setText("S"); }
            else { c->addStockpile(t); sb->setText("[S]"); }
        });
        mStockpileButtons.push_back(sb);
        row->addWidget(sb);
        x += btnW + p;

        // mothball button
        const auto mb = new eMicroButton(window());
        mb->setNoPadding();
        mb->setFontSizeXS();
        mb->setText(city && city->isShutDown(t) ? "[M]" : "M");
        mb->setWidth(btnW);
        mb->fitHeight();
        mb->setX(x);
        mb->setPressAction([&board, cid, t, mb]() {
            const auto c = board.boardCityWithId(cid);
            if(!c) return;
            if(c->isShutDown(t)) { c->removeShutDown(t); mb->setText("M"); }
            else { c->addShutDown(t); mb->setText("[M]"); }
        });
        mMothballButtons.push_back(mb);
        row->addWidget(mb);

        row->fitContent();
        addInfoWidget(row, row->height());
    }

    setCloseAction([this]() { eModal::close(); });
}
