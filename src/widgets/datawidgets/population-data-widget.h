#ifndef POPULATION_DATA_WIDGET_H
#define POPULATION_DATA_WIDGET_H

#include "edatawidget.h"

class GameBoard;
class eLabel;
class GameWidget;
class eViewModeButton;

class PopulationDataWidget : public eDataWidget {
public:
    using eDataWidget::eDataWidget;

    void initialize();
protected:
    void paintEvent(ePainter& p);
private:
    eViewModeButton* mSeeSupplies = nullptr;

    eLabel* mNewcomersLabel = nullptr;
    eLabel* mVacLabel = nullptr;

    eLabel* mPeopleDirection = nullptr;

    eWidget* mPeopleDirectionSeparator = nullptr;
    eWidget* mImiLimitedW = nullptr;
    eWidget* mImiLimitedTitleW = nullptr;
    eWidget* mImiLimitedReasonW1 = nullptr;
    eWidget* mImiLimitedReasonW2 = nullptr;
    eWidget* mImiLimitedReasonW3 = nullptr;
    eLabel* mImiLimitedReason1 = nullptr;
    eLabel* mImiLimitedReason2 = nullptr;
    eLabel* mImiLimitedReason3 = nullptr;
    eWidget* mNewcomersW = nullptr;
};

#endif // POPULATION_DATA_WIDGET_H
