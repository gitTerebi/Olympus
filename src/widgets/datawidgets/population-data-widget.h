#ifndef POPULATION_DATA_WIDGET_H
#define POPULATION_DATA_WIDGET_H

#include "edatawidget.h"

class GameBoard;
class eLabel;
class eMultiLineLabel;
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

    eWidget* mImiLimitedW = nullptr;
    eLabel* mImiLimitedReason = nullptr;
    eWidget* mNewcomersW = nullptr;
};

#endif // POPULATION_DATA_WIDGET_H
