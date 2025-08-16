#include "esupplydemandchangeeventwidget.h"

#include "widgets/eresourcebutton.h"
#include "widgets/elabeledwidget.h"
#include "widgets/ecitybutton.h"
#include "widgets/evaluebutton.h"
#include "elanguage.h"

#include "gameEvents/esupplydemandchangeevent.h"

void eSupplyDemandChangeEventWidget::initialize(
        eSupplyDemandChangeEvent* const e) {
    const auto cityButtonL = new eLabeledWidget(window());
    const auto cc = e->city();
    const auto cityButton = new eCityButton(window());
    const auto board = e->worldBoard();
    cityButton->initialize(board, [e](const stdsptr<eWorldCity>& c){
        e->setCity(c);
    });
    cityButton->setCity(cc);
    cityButtonL->setup(eLanguage::text("city:"), cityButton);
    addWidget(cityButtonL);


    const auto resourceCountButton = new eValueButton(window());
    resourceCountButton->setValueChangeAction([e](const int p) {
        e->setBy(p);
    });
    resourceCountButton->initialize(-99, 99);
    const int rc = e->by();
    resourceCountButton->setValue(rc);
    addWidget(resourceCountButton);

    for(int i = 0; i < 3; i++) {
        const auto resourceTypeButton = new eResourceButton(window());
        resourceTypeButton->initialize([i, e](const eResourceType r){
            e->setResourceType(i, r);
        }, eResourceType::allBasic, true, true);
        const auto rr = e->resourceType(i);
        resourceTypeButton->setResource(rr);
        addWidget(resourceTypeButton);
    }

    const int p = padding();
    stackVertically(p);
    fitContent();
}
