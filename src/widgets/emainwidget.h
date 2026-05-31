#ifndef EMAINWIDGET_H
#define EMAINWIDGET_H

#include "ewidget.h"

class eMainWidget : public eWidget {
public:
    using eWidget::eWidget;

    virtual void openDialog(eWidget* const w) = 0;
};

#endif // EMAINWIDGET_H
