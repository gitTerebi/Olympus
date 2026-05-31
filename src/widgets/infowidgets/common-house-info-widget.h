#ifndef ECOMMONHOUSEINFOWIDGET_H
#define ECOMMONHOUSEINFOWIDGET_H

#include "einfowidget.h"

class eHouseBase;

class CommonHouseInfoWidget : public eInfoWidget {
public:
    CommonHouseInfoWidget(eMainWindow* const window,
                           eMainWidget* const mw);

    void initialize(eHouseBase* const house);
};

#endif // ECOMMONHOUSEINFOWIDGET_H
