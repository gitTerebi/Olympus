#ifndef ETRIREMEWHARFINFOWIDGET_H
#define ETRIREMEWHARFINFOWIDGET_H

#include "eemployingbuildinginfowidget.h"

class eTriremeWharf;

class eTriremeWharfInfoWidget : public eEmployingBuildingInfoWidget {
public:
    using eEmployingBuildingInfoWidget::eEmployingBuildingInfoWidget;

    void initialize(eTriremeWharf* const b);
};

#endif // ETRIREMEWHARFINFOWIDGET_H
