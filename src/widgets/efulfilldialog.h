#ifndef EFULFILLDIALOG_H
#define EFULFILLDIALOG_H

#include "infowidgets/einfowidget.h"

class GameBoard;
class eWorldCity;

class eFulfillDialog : public eInfoWidget {
public:
    eFulfillDialog(eMainWindow* const window,
                   eMainWidget* const mw);

    using eCity = std::shared_ptr<eWorldCity>;
    void initialize(GameBoard* const board,
                    const eCity& city);
};

#endif // EFULFILLDIALOG_H
