#include "ehusbandrydatawidget.h"

#include "engine/game-board.h"

#include "eviewmodebutton.h"

#include "language.h"
#include "widgets/elinewidget.h"
#include "widgets/emultilinelabel.h"

void eHusbandryDataWidget::initialize() {
    {
        mSeeHusbandry = new eViewModeButton(
                        Language::zeusText(14, 2),
                        eViewMode::husbandry,
                        window());
        addViewButton(mSeeHusbandry);
    }

    eDataWidget::initialize();

    const auto iw = innerWidget();
    const int iww = iw->width();

    const auto canSupport1 = new eLabel(window());
    canSupport1->setWrapWidth(iww);
    canSupport1->setWrapAlignment(Alignment::hcenter);
    canSupport1->setFontSizeXS();
    canSupport1->setNoPadding();
    canSupport1->setText(Language::zeusText(57, 1)); // current food productioin can support
    canSupport1->fitContent();
    iw->addWidget(canSupport1);
    canSupport1->align(Alignment::hcenter);

    mCanSupportLabel = new eLabel("0", window());
    mCanSupportLabel->setNoPadding();
    mCanSupportLabel->setYellowFontColor();
    mCanSupportLabel->fitContent();
    iw->addWidget(mCanSupportLabel);
    mCanSupportLabel->align(Alignment::hcenter);

    const auto canSupport2 = new eLabel(window());
    canSupport2->setWrapWidth(iww);
    canSupport2->setWrapAlignment(Alignment::hcenter);
    canSupport2->setFontSizeXS();
    canSupport2->setNoPadding();
    canSupport2->setText(Language::zeusText(57, 2)); // people
    canSupport2->fitContent();
    iw->addWidget(canSupport2);
    canSupport2->align(Alignment::hcenter);

    mOpinionLabel = new eLabel(window());
    mOpinionLabel->setNoPadding();
    mOpinionLabel->setYellowFontColor();
    mOpinionLabel->setFontSizeXS();
    mOpinionLabel->setText(Language::zeusText(57, 3)); // far too little
    mOpinionLabel->fitContent();
    iw->addWidget(mOpinionLabel);
    mOpinionLabel->align(Alignment::hcenter);

    const auto spacer1 = new eWidget(window());
    spacer1->setHeight(spacing());
    iw->addWidget(spacer1);

    const auto l1 = new eLineWidget(window());
    l1->setNoPadding();
    l1->fitContent();
    l1->setWidth(iw->width());
    iw->addWidget(l1);

    const auto spacer2 = new eWidget(window());
    spacer2->setHeight(spacing());
    iw->addWidget(spacer2);

    const auto storedFood1 = new eLabel(window());
    storedFood1->setWrapWidth(iww);
    storedFood1->setWrapAlignment(Alignment::hcenter);
    storedFood1->setFontSizeXS();
    storedFood1->setNoPadding();
    storedFood1->setText(Language::zeusText(57, 27)); // stored food for
    storedFood1->fitContent();
    iw->addWidget(storedFood1);
    storedFood1->align(Alignment::hcenter);

    mStoredFoodLabel = new eLabel("0", window());
    mStoredFoodLabel->setNoPadding();
    mStoredFoodLabel->setYellowFontColor();
    mStoredFoodLabel->fitContent();
    iw->addWidget(mStoredFoodLabel);
    mStoredFoodLabel->align(Alignment::hcenter);

    const auto storedFood2 = new eLabel(window());
    storedFood2->setWrapWidth(iww);
    storedFood2->setWrapAlignment(Alignment::hcenter);
    storedFood2->setFontSizeXS();
    storedFood2->setNoPadding();
    storedFood2->setText(Language::zeusText(57, 28)); // months
    storedFood2->fitContent();
    iw->addWidget(storedFood2);
    storedFood2->align(Alignment::hcenter);

    iw->stackVertically();
}

void eHusbandryDataWidget::paintEvent(ePainter& p) {
    const bool update = refreshDue();
    if(update) {
        const auto cid = viewedCity();
        mBoard.updateResources(cid);
        const auto husbData = mBoard.husbandryData(cid);

        if(husbData) {
            const int a = husbData->canSupport();
            mCanSupportLabel->setText(std::to_string(a));
            mCanSupportLabel->fitContent();
            mCanSupportLabel->align(Alignment::hcenter);

            std::string txt;
            const int pop = mBoard.population(cid);
            if(pop == 0 || a < 0.75*pop) {
                txt = Language::zeusText(57, 3); // far too litte
            } else if(a < 0.85*pop) {
                txt = Language::zeusText(57, 4);  // much too little
            } else if(a < 0.95*pop) {
                txt = Language::zeusText(57, 5); // too little
            } else if(a < 1.10*pop) {
                txt = Language::zeusText(57, 6); // just enough
            } else if(a < 1.35*pop) {
                txt = Language::zeusText(57, 7); // plenty
            } else {
                txt = Language::zeusText(57, 8); // surplus
            }
            mOpinionLabel->setText(txt);
            mOpinionLabel->fitContent();
            mOpinionLabel->align(Alignment::hcenter);

            const int v = husbData->storedFood();
            mStoredFoodLabel->setText(std::to_string(v));
            mStoredFoodLabel->fitContent();
            mStoredFoodLabel->align(Alignment::hcenter);
        }
    }
    eWidget::paintEvent(p);
}

