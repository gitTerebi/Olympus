#ifndef EBASICCITYEVENTWIDGET_H
#define EBASICCITYEVENTWIDGET_H

#include "../ewidget.h"

class eCityEvent;
class eWorldBoard;

class eCityEventWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(eCityEvent* const e,
                    eWorldBoard &world);
};

#endif // EBASICCITYEVENTWIDGET_H
