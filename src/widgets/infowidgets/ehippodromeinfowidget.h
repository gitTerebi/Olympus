#ifndef EHIPPODROMEINFOWIDGET_H
#define EHIPPODROMEINFOWIDGET_H

#include "einfowidget.h"

class eHippodrome;

class eHippodromeInfoWidget : public eInfoWidget {
public:
    using eInfoWidget::eInfoWidget;

    void initialize(eHippodrome* const h);
};

#endif // EHIPPODROMEINFOWIDGET_H
