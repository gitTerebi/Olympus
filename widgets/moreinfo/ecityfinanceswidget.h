#ifndef ECITYFINANCESWIDGET_H
#define ECITYFINANCESWIDGET_H

#include "../infowidgets/einfowidget.h"

class eGameBoard;
enum class eCityId;

class eCityFinancesWidget : public eInfoWidget {
public:
    eCityFinancesWidget(eMainWindow* const window,
                        eMainWidget* const mw);

    void initialize(eGameBoard& board, const eCityId cid);
};

#endif // ECITYFINANCESWIDGET_H
