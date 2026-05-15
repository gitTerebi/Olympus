#ifndef STORAGEDISTRIBUTIONDIALOG_H
#define STORAGEDISTRIBUTIONDIALOG_H

#include "../infowidgets/einfowidget.h"

class eGameBoard;
class eMicroButton;
enum class eCityId;
enum class eResourceType;

class StorageDistributionDialog : public eInfoWidget {
public:
    StorageDistributionDialog(eMainWindow* const window,
                               eMainWidget* const mw);
    void initialize(eGameBoard& board, const eCityId cid);
private:
    void updateButtons(eGameBoard& board, const eCityId cid);
    std::vector<eResourceType> mTypes;
    std::vector<eMicroButton*> mStockpileButtons;
    std::vector<eMicroButton*> mMothballButtons;
};

#endif // STORAGEDISTRIBUTIONDIALOG_H
