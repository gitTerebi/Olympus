#ifndef EMILITARYMOREINFOWIDGET_H
#define EMILITARYMOREINFOWIDGET_H

#include "infowidgets/einfowidget.h"

class eGameBoard;
enum class eCityId;

class eMilitaryMoreInfoWidget : public eInfoWidget {
public:
    eMilitaryMoreInfoWidget(eMainWindow* const window,
                            eMainWidget* const mw);

    void initialize(eGameBoard& board, const eCityId cid);
private:
    eGameBoard* mBoard = nullptr;
};

#endif // EMILITARYMOREINFOWIDGET_H
