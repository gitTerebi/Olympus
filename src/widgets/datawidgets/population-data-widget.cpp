#include "population-data-widget.h"

#include "engine/game-board.h"

#include "eviewmodebutton.h"
#include "widgets/elinewidget.h"
#include "widgets/elayouthelpers.h"

#include "language.h"

void PopulationDataWidget::initialize() {
    mSeeSupplies = new eViewModeButton(
                     Language::zeusText(14, 1),
                     eViewMode::supplies,
                     window());
    addViewButton(mSeeSupplies);

    eDataWidget::initialize();

    const auto inner = innerWidget();
    const int iw = inner->width();

    const int pp = spacing();

    const auto cw1 = new eWidget(window());
    {
        cw1->setNoPadding();

        const auto hfor1 = new eLabel(window());
        hfor1->setFontSizeXS();
        hfor1->setNoPadding();
        hfor1->setText(Language::zeusText(55, 8)); // housing for
        hfor1->fitContent();
        cw1->addWidget(hfor1);

        mVacLabel = new eLabel("0", window());
        mVacLabel->setNoPadding();
        mVacLabel->setYellowFontColor();
        mVacLabel->fitContent();
        cw1->addWidget(mVacLabel);

        const auto hfor2 = new eLabel(window());
        hfor2->setFontSizeXS();
        hfor2->setNoPadding();
        hfor2->setText(Language::zeusText(55, 9)); // more people
        hfor2->fitContent();
        cw1->addWidget(hfor2);

        cw1->stackVertically();
        cw1->fitContent();
        cw1->setWidth(iw);

        hfor1->align(Alignment::hcenter);
        mVacLabel->align(Alignment::hcenter);
        hfor2->align(Alignment::hcenter);

        inner->addWidget(cw1);
    }

    const auto l1 = new eLineWidget(window());
    l1->setNoPadding();
    l1->fitContent();
    l1->setWidth(iw);
    inner->addWidget(l1);
    l1->setY(cw1->y() + cw1->height() + pp);

    {
        mPeopleDirection = new eLabel(window());
        mPeopleDirection->setWrapWidth(iw);
        mPeopleDirection->setWrapAlignment(Alignment::hcenter);
        mPeopleDirection->setNoPadding();
        mPeopleDirection->setFontSizeXS();
        mPeopleDirection->setText(Language::zeusText(55, 20)); // people wish to come

        inner->addWidget(mPeopleDirection);
        mPeopleDirection->fitContent();
        mPeopleDirection->align(Alignment::hcenter);
        mPeopleDirection->setY(l1->y() + l1->height() + pp);
    }

    const auto l2 = new eLineWidget(window());
    l2->setNoPadding();
    l2->fitContent();
    l2->setWidth(iw);
    inner->addWidget(l2);
    l2->setY(mPeopleDirection->y() + mPeopleDirection->height() + pp);
    mPeopleDirectionSeparator = l2;

    {
        const int textW = iw - 4*pp;
        const auto il1 = new eLabel(window());
        il1->setWrapWidth(textW);
        il1->setWrapAlignment(Alignment::hcenter);
        il1->setFontSizeXS();
        il1->setNoPadding();
        il1->setText(Language::zeusText(55, 12)); // immigration limited by

        const auto makeReasonLabel = [&]() {
            const auto label = new eLabel(window());
            label->setWrapWidth(textW);
            label->setWrapAlignment(Alignment::hcenter);
            label->setYellowFontColor();
            label->setFontSizeXS();
            label->setNoPadding();
            return label;
        };
        mImiLimitedReason1 = makeReasonLabel();
        mImiLimitedReason2 = makeReasonLabel();
        mImiLimitedReason3 = makeReasonLabel();

        mImiLimitedTitleW = eLayoutHelpers::createFlexContainer(
            window(), iw - 2*pp, 0, eLayoutHelpers::eFlexDirection::column,
            {{il1, 0, 0}}, {.align = eLayoutHelpers::eAlign::stretch});
        mImiLimitedReasonW1 = eLayoutHelpers::createFlexContainer(
            window(), iw - 2*pp, 0, eLayoutHelpers::eFlexDirection::column,
            {{mImiLimitedReason1, 0, 0}}, {.align = eLayoutHelpers::eAlign::stretch});
        mImiLimitedReasonW2 = eLayoutHelpers::createFlexContainer(
            window(), iw - 2*pp, 0, eLayoutHelpers::eFlexDirection::column,
            {{mImiLimitedReason2, 0, 0}}, {.align = eLayoutHelpers::eAlign::stretch});
        mImiLimitedReasonW3 = eLayoutHelpers::createFlexContainer(
            window(), iw - 2*pp, 0, eLayoutHelpers::eFlexDirection::column,
            {{mImiLimitedReason3, 0, 0}}, {.align = eLayoutHelpers::eAlign::stretch});

        mImiLimitedW = new eWidget(window());
        mImiLimitedW->setNoPadding();
        mImiLimitedW->setWidth(iw - 2*pp);
        mImiLimitedW->setX(pp);
        mImiLimitedW->addWidget(mImiLimitedTitleW);
        mImiLimitedW->addWidget(mImiLimitedReasonW1);
        mImiLimitedW->addWidget(mImiLimitedReasonW2);
        mImiLimitedW->addWidget(mImiLimitedReasonW3);

        inner->addWidget(mImiLimitedW);
        mImiLimitedW->setY(l2->y() + l2->height() + pp);
    }


    const auto cw4 = new eWidget(window());
    {
        cw4->setNoPadding();

        mNewcomersLabel = new eLabel("0", window());
        mNewcomersLabel->setNoPadding();
        mNewcomersLabel->setYellowFontColor();
        mNewcomersLabel->fitContent();
        cw4->addWidget(mNewcomersLabel);

        const auto il1 = new eLabel(window());
        il1->setWrapWidth(iw);
        il1->setWrapAlignment(Alignment::hcenter);
        il1->setFontSizeXS();
        il1->setNoPadding();
        il1->setText(Language::zeusText(55, 10)); // newcomers arrived this month
        il1->fitContent();
        cw4->addWidget(il1);

        cw4->stackVertically();
        cw4->fitContent();
        cw4->setWidth(iw - 2*pp);

        il1->align(Alignment::hcenter);
        mNewcomersLabel->align(Alignment::hcenter);

        inner->addWidget(cw4);
        cw4->setY(l2->y() + l2->height() + pp);

        mNewcomersW = cw4;
        mNewcomersW->hide();
    }
}

void PopulationDataWidget::paintEvent(ePainter& p) {
    const bool update = ((mTime++) % 20) == 0;
    if(update) {
        const int pp = spacing();
        const auto cid = viewedCity();
        const auto popData = mBoard.populationData(cid);

        if(popData) {
            const int a = popData->arrived();
            mNewcomersW->setVisible(a > 0);
            mNewcomersLabel->setText(std::to_string(a));
            mNewcomersLabel->fitContent();
            mNewcomersLabel->align(Alignment::hcenter);

            const auto limit = mBoard.immigrationLimit(cid);
            const bool activeInvasion = mBoard.hasActiveInvasions(cid);
            const bool pendingInvasion = mBoard.invasionToDefend(cid);
            const bool war = activeInvasion || pendingInvasion;

            const int l = popData->left();
            std::string pdtxt;
            if(war || limit != eImmigrationLimitedBy::none) {
                pdtxt = Language::zeusText(55, 24); // immigrants aren't coming
            } else if(l > a) {
                pdtxt = Language::zeusText(55, 21); // people are leaving the city
            } else if(a > l) {
                pdtxt = Language::zeusText(55, 20); // people wish to come to the city
            } else {
                pdtxt = Language::zeusText(55, 22); // population migration is stable
            }
            mPeopleDirection->setText(pdtxt);
            mPeopleDirection->fitContent();
            mPeopleDirection->align(Alignment::hcenter);
            mPeopleDirectionSeparator->setY(
                mPeopleDirection->y() + mPeopleDirection->height() + pp);

            const int v = popData->vacancies();
            mVacLabel->setText(std::to_string(v));
            mVacLabel->fitContent();
            mVacLabel->align(Alignment::hcenter);

            std::string ilrtxt1;
            std::string ilrtxt2;
            std::string ilrtxt3;
            if(war) {
                ilrtxt1 = "The threat of";
                ilrtxt2 = "war scares";
                ilrtxt3 = "immigrants";
            } else if(v <= 0) {
                ilrtxt1 = Language::zeusText(55, 13); // lack of housing vacancies
            } else if(limit == eImmigrationLimitedBy::lowWages) {
                ilrtxt1 = Language::zeusText(55, 14);
            } else if(limit == eImmigrationLimitedBy::unemployment) {
                ilrtxt1 = Language::zeusText(55, 15);
            } else if(limit == eImmigrationLimitedBy::lackOfFood) {
                ilrtxt1 = Language::zeusText(55, 16);
            } else if(limit == eImmigrationLimitedBy::highTaxes) {
                ilrtxt1 = Language::zeusText(55, 17);
            } else if(limit == eImmigrationLimitedBy::prolongedDebt) {
                ilrtxt1 = Language::zeusText(55, 18);
            } else if(limit == eImmigrationLimitedBy::excessiveMilitaryService) {
                ilrtxt1 = Language::zeusText(55, 19);
            } else if(limit == eImmigrationLimitedBy::unpopularity) {
                ilrtxt1 = Language::zeusText(54, 58);
            }
            const auto setupLine = [&](eWidget* const w, eLabel* const label,
                                       const std::string& text, const int y) {
                label->setText(text);
                label->setWrapWidth(mImiLimitedW->width());
                label->fitContent();
                label->align(Alignment::hcenter);
                w->setHeight(label->height());
                w->setVisible(!text.empty());
                w->setY(y);
                return text.empty() ? y : y + w->height();
            };
            int y = 0;
            mImiLimitedTitleW->setY(y);
            y += mImiLimitedTitleW->height() + pp;
            y = setupLine(mImiLimitedReasonW1, mImiLimitedReason1, ilrtxt1, y);
            y = setupLine(mImiLimitedReasonW2, mImiLimitedReason2, ilrtxt2, y);
            y = setupLine(mImiLimitedReasonW3, mImiLimitedReason3, ilrtxt3, y);
            mImiLimitedW->setHeight(y);
            const bool hasReason = !ilrtxt1.empty() ||
                                   !ilrtxt2.empty() ||
                                   !ilrtxt3.empty();
            const bool showLimited = hasReason && (a <= 0 || war);
            mImiLimitedW->setVisible(showLimited);
            mNewcomersW->setVisible(a > 0 && !showLimited);
            mImiLimitedW->setY(
                mPeopleDirectionSeparator->y() +
                mPeopleDirectionSeparator->height() + pp);
            mNewcomersW->setY(mImiLimitedW->y());
        }
    }
    eWidget::paintEvent(p);
}
