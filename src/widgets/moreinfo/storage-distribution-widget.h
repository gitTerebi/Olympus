#ifndef STORAGEDISTRIBUTIONWIDGET_H
#define STORAGEDISTRIBUTIONWIDGET_H

#include "../infowidgets/einfowidget.h"

class GameBoard;
class eMicroButton;
enum class eCityId;
enum class eResourceType;

class StorageDistributionWidget : public eInfoWidget {
public:
    StorageDistributionWidget(MainWindow* const window,
                              eMainWidget* const mw);
    void initialize(GameBoard& board, const eCityId cid);
private:
    std::vector<eResourceType> mTypes;
    std::vector<eMicroButton*> mStockpileButtons;
    std::vector<eMicroButton*> mMothballButtons;
};

#endif // STORAGEDISTRIBUTIONWIDGET_H
