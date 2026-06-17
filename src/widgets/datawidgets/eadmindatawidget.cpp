#include "eadmindatawidget.h"

#include "engine/game-board.h"

#include "../elabel.h"

#include "../eupbutton.h"
#include "../edownbutton.h"

#include "eviewmodebutton.h"

#include "language.h"
#include "widgets/emultilinelabel.h"
#include "widgets/moreinfo/city-finances-widget.h"

void eAdminDataWidget::initialize() {
    {
        mSeeTaxes = new eViewModeButton(
                        Language::zeusText(14, 9), // see taxes
                        eViewMode::taxes,
                        window());
        addViewButton(mSeeTaxes);
    }

    eDataWidget::initialize();

    const auto inner = innerWidget();
    const int iw = inner->width();

    {
        const auto l = new eLabel(window());
        l->setNoPadding();
        l->setFontSizeXS();
        l->setText(Language::zeusText(131, 0)); // tax rate
        l->fitContent();
        inner->addWidget(l);
        l->align(Alignment::hcenter);

        mTaxLabel = new eLabel(window());
        mTaxLabel->setNoPadding();
        mTaxLabel->setYellowFontColor();
        mTaxLabel->setFontSizeXS();
        mTaxLabel->setText(eTaxRateHelpers::name(eTaxRate::normal));
        mTaxLabel->fitContent();
        inner->addWidget(mTaxLabel);
        mTaxLabel->align(Alignment::hcenter);

        const auto w = new eWidget(window());
        w->setNoPadding();
        const auto downButton = new eDownButton(window());
        downButton->setPressAction([this]() {
            if(mTaxRate == eTaxRate::none) return;
            const int wr = static_cast<int>(mTaxRate) - 1;
            setTaxRate(static_cast<eTaxRate>(wr));
        });
        w->addWidget(downButton);
        const auto upButton = new eUpButton(window());
        upButton->setPressAction([this]() {
            if(mTaxRate == eTaxRate::outrageous) return;
            const int wr = static_cast<int>(mTaxRate) + 1;
            setTaxRate(static_cast<eTaxRate>(wr));
        });
        w->addWidget(upButton);
        w->stackHorizontally();
        w->fitContent();
        inner->addWidget(w);
        w->align(Alignment::hcenter);
    }

    {
        mYields = new eLabel(window());
        mYields->setWrapWidth(iw);
        mYields->setWrapAlignment(Alignment::hcenter);
        mYields->setNoPadding();
        mYields->setFontSizeXS();
        const auto yt = Language::zeusText(60, 4); // yields an estimated
        const auto dr = Language::zeusText(8, 1);
        mYields->setText(yt + " 0 " + dr);
        mYields->fitContent();
        inner->addWidget(mYields);
        mYields->align(Alignment::hcenter);
    }

    {
        mPerPop = new eLabel(window());
        mPerPop->setWrapWidth(iw);
        mPerPop->setWrapAlignment(Alignment::hcenter);
        mPerPop->setNoPadding();
        mPerPop->setFontSizeXS();
        const auto pt = Language::zeusText(60, 5); // of population visited by clerk
        mPerPop->setText("0% " + pt);
        mPerPop->fitContent();
        inner->addWidget(mPerPop);
        mPerPop->align(Alignment::hcenter);
    }

    inner->stackVertically();

    showMoreInfoButton();
}

void eAdminDataWidget::setTaxRate(const eTaxRate tr) {
    const auto cid = viewedCity();
    mBoard.setTaxRate(cid, tr);
    mTaxRate = tr;
    mTaxLabel->setText(eTaxRateHelpers::name(tr));
    mTaxLabel->fitContent();
    mTaxLabel->align(Alignment::hcenter);
}

void eAdminDataWidget::paintEvent(ePainter& p) {
    const bool update = refreshDue();
    if(update) {
        const auto cid = viewedCity();
        {
            const auto yt = Language::zeusText(60, 4); // yields an estimated
            const int y = mBoard.taxesPaidLastYear(cid);
            const auto dr = Language::zeusText(8, 1);
            mYields->setText(yt + " " + std::to_string(y) + " " + dr);
            mYields->fitContent();
            mYields->align(Alignment::hcenter);
        }
        {
            const auto pt = Language::zeusText(60, 5); // of population visited by clerk
            const int paid = mBoard.peoplePaidTaxesLastYear(cid);
            const int pop = mBoard.population(cid);
            int per = pop == 0 ? 0 : std::round(100.*paid/pop);
            per = std::clamp(per, 0, 100);
            mPerPop->setText(std::to_string(per) + "% " + pt);
            mPerPop->fitContent();
            mPerPop->align(Alignment::hcenter);
        }
        const auto inner = innerWidget();
        inner->stackVertically();
    }
    eWidget::paintEvent(p);
}

void eAdminDataWidget::openMoreInfoWiget() {
    const auto cid = viewedCity();
    if(cid == eCityId::neutralFriendly ||
        cid == eCityId::neutralAggresive) return;
    const auto gw = gameWidget();
    const auto w = new eCityFinancesWidget(
        window(), gw);
    w->initialize(mBoard, cid);
    gw->openDialog(w);
}
