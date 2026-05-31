#ifndef ConditionSettingsWidget_H
#define ConditionSettingsWidget_H

#include "eframedwidget.h"

#include "engine/ai/ai-district.h"

class ConditionSettingsWidget : public eFramedWidget {
public:
    using eFramedWidget::eFramedWidget;

    using eSetter = std::function<void(const eDistrictReadyCondition&)>;
    void initialize(const eDistrictReadyCondition& ini,
                    const eSetter& setter);
};

#endif // ConditionSettingsWidget_H
