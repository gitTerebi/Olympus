#ifndef EGAMEMAINMENU_H
#define EGAMEMAINMENU_H

#include "eframedwidget.h"

class GameBoard;

class eGameMainMenu : public eFramedWidget {
public:
    using eFramedWidget::eFramedWidget;

    using eAction = std::function<void()>;
    void initialize(const eAction& resumeAct,
                    const eAction& saveAct,
                    const eAction& loadAct,
                    const eAction& optionsAct,
                    const eAction& exitAct);
};

#endif // EGAMEMAINMENU_H
