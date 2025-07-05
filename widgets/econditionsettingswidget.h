#ifndef ECONDITIONSETTINGSWIDGET_H
#define ECONDITIONSETTINGSWIDGET_H

#include "eframedwidget.h"

#include "engine/ai/eaidistrict.h"

class eConditionSettingsWidget : public eFramedWidget {
public:
    using eFramedWidget::eFramedWidget;

    using eSetter = std::function<void(const eDistrictReadyCondition&)>;
    void initialize(const eDistrictReadyCondition& ini,
                    const eSetter& setter);
};

#endif // ECONDITIONSETTINGSWIDGET_H
