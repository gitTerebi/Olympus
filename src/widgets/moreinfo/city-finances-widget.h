#ifndef CITYFINANCESWIDGET_H
#define CITYFINANCESWIDGET_H

#include "../infowidgets/einfowidget.h"

class GameBoard;
enum class eCityId;

class eCityFinancesWidget : public eInfoWidget {
public:
    eCityFinancesWidget(MainWindow* const window,
                        eMainWidget* const mw);

    void initialize(GameBoard& board, const eCityId cid);
};

#endif // CITYFINANCESWIDGET_H
