#ifndef EINVASIONEVENTWIDGET_H
#define EINVASIONEVENTWIDGET_H

#include "../ewidget.h"

#include "gameEvents/einvasionevent.h"

class eLabeledWidget;

class eInvasionEventWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(eInvasionEvent* const e);
private:
    bool mHardcoded = true;
};

#endif // EINVASIONEVENTWIDGET_H
