#include "ebasiccityeventwidget.h"

#include "widgets/elabeledwidget.h"
#include "widgets/ecitybutton.h"
#include "elanguage.h"

#include "gameEvents/ecityevent.h"

void eCityEventWidget::initialize(eCityEvent* const e,
                                       eWorldBoard& world) {
    const auto cityButtonL = new eLabeledWidget(window());
    const auto cc = e->city();
    const auto cityButton = new eCityButton(window());
    cityButton->initialize(&world, [e](const stdsptr<eWorldCity>& c){
        e->setCity(c);
    });
    cityButton->setCity(cc);
    cityButtonL->setup(eLanguage::text("city:"), cityButton);
    addWidget(cityButtonL);

    const int p = padding();
    stackVertically(p);
    fitContent();
}
