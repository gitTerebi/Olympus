#include "egoddisastereventwidget.h"

#include "widgets/elabeledwidget.h"
#include "widgets/evaluebutton.h"
#include "language.h"

#include "gameEvents/gods/egoddisasterevent.h"

void eGodDisasterEventWidget::initialize(eGodDisasterEvent* const e) {
    const auto durButtonL = new eLabeledWidget(window());
    const auto durButton = new eValueButton(window());
    durButton->setValueChangeAction([e](const int d) {
        e->setDuration(d);
    });
    durButton->initialize(0, 9999);
    durButton->setValue(e->duration());
    durButtonL->setup(Language::zeusText(44, 356), durButton);
    addWidget(durButtonL);

    const int p = padding();
    stackVertically(p);
    fitContent();
}
