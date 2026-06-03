#ifndef ECITYSETTINGSWIDGET_H
#define ECITYSETTINGSWIDGET_H

#include "eframedwidget.h"
#include "engine/world-city.h"

class eCitySettingsWidget : public eFramedWidget {
public:
    using eFramedWidget::eFramedWidget;

    void initialize(const stdsptr<WorldCity>& c,
                    GameBoard* const board,
                    WorldBoard* const wb);
};

#endif // ECITYSETTINGSWIDGET_H
