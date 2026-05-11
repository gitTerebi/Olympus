#ifndef STORAGEDISTRIBUTIONWIDGET_H
#define STORAGEDISTRIBUTIONWIDGET_H

#include "../infowidgets/einfowidget.h"

class eGameBoard;
class eMicroButton;
enum class eCityId;
enum class eResourceType;

class StorageDistributionWidget : public eInfoWidget {
public:
    StorageDistributionWidget(eMainWindow* const window,
                               eMainWidget* const mw);
    void initialize(eGameBoard& board, const eCityId cid);
private:
    void updateButtons(eGameBoard& board, const eCityId cid);
    std::vector<eResourceType> mTypes;
    std::vector<eMicroButton*> mStockpileButtons;
    std::vector<eMicroButton*> mMothballButtons;
};

#endif // STORAGEDISTRIBUTIONWIDGET_H
