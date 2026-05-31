#ifndef ESANCTUARYINFOWIDGET_H
#define ESANCTUARYINFOWIDGET_H

#include "eemployingbuildinginfowidget.h"

class eMonument;

class eSanctuaryInfoWidget : public eEmployingBuildingInfoWidget {
public:
    eSanctuaryInfoWidget(eMainWindow* const window,
                         eMainWidget* const mw);

    void initialize(eMonument* const s);
};

#endif // ESANCTUARYINFOWIDGET_H
