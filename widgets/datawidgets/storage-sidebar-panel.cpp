#include "storage-sidebar-panel.h"

#include "engine/e-game-board.h"
#include "widgets/elabel.h"
#include "widgets/game-widget.h"
#include "widgets/moreinfo/storage-distribution-dialog.h"
#include "eviewmodebutton.h"

#include "elanguage.h"

namespace {
class eClickRow : public eWidget {
public:
    eClickRow(eMainWindow* w, std::function<void()> onClick)
        : eWidget(w), mOnClick(std::move(onClick)) { setNoPadding(); }
protected:
    bool mousePressEvent(const eMouseEvent&) override {
        if(mOnClick) mOnClick();
        return true;
    }
private:
    std::function<void()> mOnClick;
};
}

eWidget* StorageSidebarPanel::sdwColumn(
        const eUIScale uiScale,
        const int iMin, const int iMax,
        const std::vector<eResourceType>& tps,
        std::vector<eLabel*>& lbls) {
    const int pp = spacing();
    const auto w0 = new eWidget(window());
    w0->setNoPadding();
    for(int i = iMin; i < iMax; i++) {
        const auto t = tps[i];
        const auto icon = eResourceTypeHelpers::icon(uiScale, t);
        auto& board = mBoard;
        const auto cidProvider = [this]() { return viewedCity(); };
        const auto w = new eClickRow(window(), [&board, cidProvider, t]() {
            const auto c = board.boardCityWithId(cidProvider());
            if(!c) return;
            if(c->isStockpiled(t)) c->removeStockpile(t);
            else                   c->addStockpile(t);
        });
        const auto ic = new eLabel(window());
        ic->setTexture(icon);
        ic->setNoPadding();
        ic->fitContent();
        mResourceIcons.push_back(ic);
        mResourceTypes.push_back(t);

        const auto lw = new eWidget(window());
        lw->setNoPadding();
        const auto l = new eLabel("-", window());
        l->setFontSizeXS();
        l->setNoPadding();
        l->fitContent();
        lbls.push_back(l);
        lw->addWidget(l);
        l->align(eAlignment::right);

        w->addWidget(lw);
        w->addWidget(ic);
        w->stackHorizontally(2*pp);
        w->fitContent();
        ic->align(eAlignment::vcenter);
        l->align(eAlignment::vcenter);
        lw->align(eAlignment::vcenter);
        w0->addWidget(w);
    }
    w0->stackVertically(2*pp);
    w0->fitContent();
    return w0;
}

void StorageSidebarPanel::initialize() {
    {
        mSeeDistribution = new eViewModeButton(
                        eLanguage::zeusText(14, 4),
                        eViewMode::distribution,
                        window());
        addViewButton(mSeeDistribution);
    }

    eDataWidget::initialize();

    const auto inner = innerWidget();

    const auto all = eResourceType::allBasic; // mBoard.supportedResources();
    const auto tps = eResourceTypeHelpers::extractResourceTypes(all);
    const auto res = resolution();
    const auto uiScale = res.uiScale();

    const int iMin0 = 0;
    const int iMax0 = tps.size()/2 + 1;
    const auto w0 = sdwColumn(uiScale,
                              iMin0, iMax0, tps,
                              mResourceLabels);
    const int iMin1 = iMax0;
    const int iMax1 = tps.size();
    const auto w1 = sdwColumn(uiScale,
                              iMin1, iMax1, tps,
                              mResourceLabels);

    const int pp = spacing();

    const auto w = new eWidget(window());
    w->setNoPadding();
    w->addWidget(w0);
    w->addWidget(w1);
    w->stackHorizontally(10*pp);
    w->fitContent();

    inner->addWidget(w);
    w->align(eAlignment::center);
    w->setX(w->x() + 2*pp);

    showMoreInfoButton();
}

void StorageSidebarPanel::openMoreInfoWiget() {
    const auto gw = gameWidget();
    const auto w = new StorageDistributionDialog(window(), gw);
    w->initialize(mBoard, viewedCity());
    gw->openDialog(w);
}

void StorageSidebarPanel::paintEvent(ePainter& p) {
    const bool update = ((mTime++) % 20) == 0;
    if(update) {
        const auto cid = viewedCity();
        mBoard.updateResources(cid);
        const auto& src = mBoard.resources(cid);
        if(src) {
            const int iMax = mResourceLabels.size();
            for(int i = 0; i < iMax; i++) {
                const auto c = (*src)[i].second;
                const auto l = mResourceLabels[i];
                l->setText(std::to_string(c));
                l->fitContent();
                l->align(eAlignment::right);
            }
        }
        const auto city = mBoard.boardCityWithId(cid);
        const int iMax = mResourceIcons.size();
        for(int i = 0; i < iMax; i++) {
            const auto t = mResourceTypes[i];
            const bool s = city && city->isStockpiled(t);
            const auto ic = mResourceIcons[i];
            const auto l = mResourceLabels[i];
            if(s) {
                ic->setTextureColorMod(128, 128, 128);
                l->setDarkFontColor();
            } else {
                ic->setTextureColorMod(255, 255, 255);
                l->setLightFontColor();
            }
        }
    }
    eWidget::paintEvent(p);
}
