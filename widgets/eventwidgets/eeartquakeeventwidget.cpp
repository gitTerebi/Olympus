#include "eeartquakeeventwidget.h"

#include "widgets/elabeledwidget.h"
#include "widgets/evaluebutton.h"
#include "elanguage.h"

#include "widgets/egodbutton.h"

#include "gameEvents/eearthquakeevent.h"
#include "epointeventbasewidget.h"

void eEartquakeEventWidget::initialize(eEarthquakeEvent* const e) {
    const int p = padding();

    const auto godButtonL = new eLabeledWidget(window());
    const auto act = [e](const eGodType type) {
        e->setGod(type);
    };
    const auto godButton = new eGodButton(window());
    godButton->initialize(act);
    const auto iniT = e->god();
    godButton->setType(iniT);
    godButtonL->setup(eLanguage::text("god:"), godButton);
    addWidget(godButtonL);

    const auto sizeButtonL = new eLabeledWidget(window());
    const auto sizeButton = new eValueButton(window());
    sizeButton->setValueChangeAction([e](const int s) {
        e->setSize(s);
    });
    sizeButton->initialize(0, 9999);
    sizeButton->setValue(e->size());
    sizeButtonL->setup(eLanguage::text("size:"), sizeButton);
    addWidget(sizeButtonL);

    ePointEventBaseWidget::add(this, e);

    stackVertically(p);
    fitContent();
}
