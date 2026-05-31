#ifndef EBOARDSETTINGSMENU_H
#define EBOARDSETTINGSMENU_H

#include "eframedwidget.h"

class GameBoard;
class GameWidget;

class eBoardSettingsMenu : public eFramedWidget {
public:
    using eFramedWidget::eFramedWidget;

    void initialize(GameWidget* const gw, GameBoard& board);
};

#endif // EBOARDSETTINGSMENU_H
