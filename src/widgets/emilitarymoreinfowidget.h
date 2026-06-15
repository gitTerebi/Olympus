#ifndef EMILITARYMOREINFOWIDGET_H
#define EMILITARYMOREINFOWIDGET_H

#include "infowidgets/einfowidget.h"

class GameBoard;
enum class eCityId;

class eMilitaryMoreInfoWidget : public eInfoWidget {
public:
    eMilitaryMoreInfoWidget(MainWindow* const window,
                            eMainWidget* const mw);

    void initialize(GameBoard& board, const eCityId cid);
private:
    GameBoard* mBoard = nullptr;
};

#endif // EMILITARYMOREINFOWIDGET_H
