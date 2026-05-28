#ifndef EMILITARYDATAWIDGET_H
#define EMILITARYDATAWIDGET_H

#include "edatawidget.h"
#include "pointers/estdselfref.h"

class GameBoard;
class SoldierBanner;
class eMicroButton;
class eScrollWidget;

using eSB = stdptr<SoldierBanner>;
using SoldierBanners = std::vector<eSB>;
using SoldierBannerAction = std::function<void(const eSB&)>;

class eForcesWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const std::string& title);

    void setBanners(const SoldierBanners& ss,
                    const SoldierBannerAction& act);
private:
    eWidget* mWidget = nullptr;
    SoldierBanners mBanners;
};

class eMilitaryDataWidget : public eDataWidget {
public:
    using eDataWidget::eDataWidget;

    void initialize();
protected:
    void paintEvent(ePainter& p);
    void openMoreInfoWiget();
private:
    void updateWidgets();

    eViewModeButton* mSeeSecurity = nullptr;

    eScrollWidget* mForcesScroll = nullptr;
    eWidget* mForcesWidget = nullptr;

    eForcesWidget* mAbroad = nullptr;
    eForcesWidget* mInCity = nullptr;
    eForcesWidget* mStandingDown = nullptr;

    eMicroButton* mAtPalace = nullptr;
    eMicroButton* mNoShips = nullptr;
    eMicroButton* mNoTowers = nullptr;
};

#endif // EMILITARYDATAWIDGET_H
