#ifndef STORAGEDATAWIDGET_H
#define STORAGEDATAWIDGET_H

#include "edatawidget.h"

#include "engine/eresourcetype.h"

class eGameBoard;
class eLabel;
class eViewModeButton;

class StorageDataWidget : public eDataWidget {
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
            std::vector<eLabel*>& lbls,
            std::vector<eLabel*>& icons);

    eViewModeButton* mSeeDistribution = nullptr;

    std::vector<eLabel*> mResourceLabels;
    std::vector<eLabel*> mResourceIcons;
};

#endif // STORAGEDATAWIDGET_H
