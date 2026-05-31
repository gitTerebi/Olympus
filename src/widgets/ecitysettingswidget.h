#ifndef ECITYSETTINGSWIDGET_H
#define ECITYSETTINGSWIDGET_H

#include "eframedwidget.h"
#include "engine/e-worldcity.h"

class eCitySettingsWidget : public eFramedWidget {
public:
    using eFramedWidget::eFramedWidget;

    void initialize(const stdsptr<eWorldCity>& c,
                    GameBoard* const board,
                    eWorldBoard* const wb);
};

#endif // ECITYSETTINGSWIDGET_H
