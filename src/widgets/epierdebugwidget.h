#ifndef EPIERDEBUGWIDGET_H
#define EPIERDEBUGWIDGET_H

#include "ewidget.h"

class FramedButton;

class ePierDebugWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize();

private:
    void updateLabel(const int row);

    FramedButton* mLabels[8] = {nullptr};
};

#endif // EPIERDEBUGWIDGET_H
