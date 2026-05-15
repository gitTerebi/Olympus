#ifndef STORAGESIDEBARPANEL_H
#define STORAGESIDEBARPANEL_H

#include "edatawidget.h"

#include "engine/eresourcetype.h"

class eGameBoard;
class eLabel;
class eViewModeButton;

class StorageSidebarPanel : public eDataWidget {
public:
    using eDataWidget::eDataWidget;

    void initialize() override;
    void openMoreInfoWiget() override;
protected:
    void paintEvent(ePainter& p) override;
private:
    eWidget* sdwColumn(const eUIScale uiScale,
            const int iMin, const int iMax,
            const std::vector<eResourceType>& tps,
            std::vector<eLabel*>& lbls);

    eViewModeButton* mSeeDistribution = nullptr;

    std::vector<eLabel*> mResourceLabels;
    std::vector<eLabel*> mResourceIcons;
    std::vector<eResourceType> mResourceTypes;
};

#endif // STORAGESIDEBARPANEL_H
