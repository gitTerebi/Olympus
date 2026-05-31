#ifndef OverviewDataWidget_H
#define OverviewDataWidget_H

#include "edatawidget.h"

class GameWidget;
class eViewModeButton;
class eOverviewEntry;
class eMiniMap;
class eScrollWidget;

class OverviewDataWidget : public eDataWidget {
public:
    using eDataWidget::eDataWidget;

    void initialize() override;

    void shown() override;

    void updateRequestButtons();
    void setMap(eMiniMap* const map);
protected:
    void paintEvent(ePainter& p) override;
private:
    void addGodQuests(eWidget* const w);
    void addCityRequests(eWidget* const w);

    eViewModeButton* mSeeProblems = nullptr;
    eViewModeButton* mSeeRoads = nullptr;

    eScrollWidget* mQuestButtons = nullptr;

    eOverviewEntry* mPopularity = nullptr;
    eOverviewEntry* mFoodLevel = nullptr;
    eOverviewEntry* mUnemployment = nullptr;
    eOverviewEntry* mHygiene = nullptr;
    eOverviewEntry* mUnrest = nullptr;
    eOverviewEntry* mFinances = nullptr;

    eMiniMap* mMap = nullptr;
};

#endif // OverviewDataWidget_H
