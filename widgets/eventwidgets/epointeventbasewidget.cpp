#include "epointeventbasewidget.h"

#include "widgets/ewidget.h"
#include "widgets/evaluebutton.h"
#include "gameEvents/epointeventbase.h"

void ePointEventBaseWidget::add(eWidget * const w, ePointEventBase * const e) {
    const auto window = w->window();
    const auto countW = new eWidget(window);
    const int p = countW->padding();
    countW->setNoPadding();
    {
        const auto minCountB = new eValueButton(window);
        minCountB->setValueChangeAction([e](const int p) {
            e->setMinPointId(p);
        });
        minCountB->initialize(1, 999);
        const int rc = e->minPointId();
        minCountB->setValue(rc);
        countW->addWidget(minCountB);
    }
    {
        const auto maxCountB = new eValueButton(window);
        maxCountB->setValueChangeAction([e](const int p) {
            e->setMaxPointId(p);
        });
        maxCountB->initialize(1, 999);
        const int rc = e->maxPointId();
        maxCountB->setValue(rc);
        countW->addWidget(maxCountB);
    }

    countW->stackHorizontally(p);
    countW->fitContent();
    w->addWidget(countW);
}
