#ifndef WORKFORCEALLOCATIONWIDGET_H
#define WORKFORCEALLOCATIONWIDGET_H

#include "../infowidgets/einfowidget.h"

class GameBoard;
enum class eSector;
enum class eCityId;

class eWorkforceAllocationWidget : public eInfoWidget {
public:
    eWorkforceAllocationWidget(eMainWindow* const window,
                               eMainWidget* const mw);

    void initialize(GameBoard& board, const eCityId cid);
private:
    void updateLabels();

    GameBoard* mBoard = nullptr;
    eCityId mCityId;
    std::map<eSector, eLabel*> mEmplMaxLabels;
    std::map<eSector, eLabel*> mEmplLabels;
};

#endif // WORKFORCEALLOCATIONWIDGET_H
