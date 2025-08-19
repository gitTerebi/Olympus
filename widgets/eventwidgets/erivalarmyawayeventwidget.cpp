#include "erivalarmyawayeventwidget.h"

#include "widgets/elabeledwidget.h"
#include "widgets/evaluebutton.h"
#include "elanguage.h"

#include "gameEvents/erivalarmyawayevent.h"

void eRivalArmyAwayEventWidget::initialize(eRivalArmyAwayEvent* const e) {
    const auto durButtonL = new eLabeledWidget(window());
    const auto durButton = new eValueButton(window());
    durButton->setValueChangeAction([e](const int d) {
        e->setDuration(d);
    });
    durButton->initialize(0, 9999);
    durButton->setValue(e->duration());
    durButtonL->setup(eLanguage::text("duration:"), durButton);
    addWidget(durButtonL);

    const int p = padding();
    stackVertically(p);
    fitContent();
}
