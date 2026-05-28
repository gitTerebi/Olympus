#ifndef EBOARDSETTINGSMENU_H
#define EBOARDSETTINGSMENU_H

#include "eframedwidget.h"

class eGameBoard;
class GameWidget;

class eBoardSettingsMenu : public eFramedWidget {
public:
    using eFramedWidget::eFramedWidget;

    void initialize(GameWidget* const gw, eGameBoard& board);
};

#endif // EBOARDSETTINGSMENU_H
