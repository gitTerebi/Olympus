#ifndef ECITYSETTINGSWIDGET_H
#define ECITYSETTINGSWIDGET_H

#include "eframedwidget.h"
#include "engine/eworldcity.h"

class eCitySettingsWidget : public eFramedWidget {
public:
    using eFramedWidget::eFramedWidget;

    void initialize(const stdsptr<eWorldCity>& c,
                    eGameBoard* const board,
                    eWorldBoard* const wb);
};

#endif // ECITYSETTINGSWIDGET_H
