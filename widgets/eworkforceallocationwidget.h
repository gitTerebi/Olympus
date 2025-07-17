#ifndef EWORKFORCEALLOCATIONWIDGET_H
#define EWORKFORCEALLOCATIONWIDGET_H

#include "infowidgets/einfowidget.h"

class eGameBoard;
enum class eSector;
enum class eCityId;

class eWorkforceAllocationWidget : public eInfoWidget {
public:
    eWorkforceAllocationWidget(eMainWindow* const window,
                               eMainWidget* const mw);

    void initialize(eGameBoard& board, const eCityId cid);
private:
    void updateLabels();

    eGameBoard* mBoard = nullptr;
    eCityId mCityId;
    std::map<eSector, eLabel*> mEmplMaxLabels;
    std::map<eSector, eLabel*> mEmplLabels;
};

#endif // EWORKFORCEALLOCATIONWIDGET_H
