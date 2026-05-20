#ifndef EPIERDEBUGWIDGET_H
#define EPIERDEBUGWIDGET_H

#include "ewidget.h"

class eFramedButton;

class ePierDebugWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize();

private:
    void updateLabel(const int facing);

    eFramedButton* mLabels[4] = {nullptr, nullptr, nullptr, nullptr};
};

#endif // EPIERDEBUGWIDGET_H
