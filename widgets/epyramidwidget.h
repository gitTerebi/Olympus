#ifndef EPYRAMIDWIDGET_H
#define EPYRAMIDWIDGET_H

#include "eframedwidget.h"

struct ePyramidSettings;

class ePyramidWidget : public eFramedWidget {
public:
    using eFramedWidget::eFramedWidget;

    using ePSptr = std::shared_ptr<ePyramidSettings>;
    void initialize(const ePSptr& e, const eAction& updater);
};

#endif // EPYRAMIDWIDGET_H
