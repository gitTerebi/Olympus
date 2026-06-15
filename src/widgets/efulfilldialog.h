#ifndef EFULFILLDIALOG_H
#define EFULFILLDIALOG_H

#include "infowidgets/einfowidget.h"

class GameBoard;
class WorldCity;

class eFulfillDialog : public eInfoWidget {
public:
    eFulfillDialog(MainWindow* const window,
                   eMainWidget* const mw);

    using eCity = std::shared_ptr<WorldCity>;
    void initialize(GameBoard* const board,
                    const eCity& city);
};

#endif // EFULFILLDIALOG_H
