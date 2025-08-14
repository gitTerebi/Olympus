#include "egiftfromeventwidget.h"

#include "widgets/eresourcebutton.h"
#include "widgets/ecitybutton.h"
#include "widgets/evaluebutton.h"
#include "widgets/elabeledwidget.h"
#include "elanguage.h"

void eGiftFromEventWidget::initialize(eGiftFromEvent* const e) {
    const int p = padding();
    const auto cityButtonL = new eLabeledWidget(window());
    const auto cityButton = new eCityButton(window());
    const auto board = e->worldBoard();
    cityButton->initialize(board, [e](const stdsptr<eWorldCity>& c){
        e->setCity(c);
    });
    const auto cc = e->city();
    cityButton->setCity(cc);
    cityButtonL->setup(eLanguage::text("city:"), cityButton);
    addWidget(cityButtonL);


    {
        const auto countW = new eWidget(window());
        countW->setNoPadding();
        {
            const auto minCountB = new eValueButton(window());
            minCountB->setValueChangeAction([e](const int p) {
                e->setMinResourceCount(p);
            });
            minCountB->initialize(1, 999);
            const int rc = e->minResourceCount();
            minCountB->setValue(rc);
            countW->addWidget(minCountB);
        }
        {
            const auto maxCountB = new eValueButton(window());
            maxCountB->setValueChangeAction([e](const int p) {
                e->setMaxResourceCount(p);
            });
            maxCountB->initialize(1, 999);
            const int rc = e->maxResourceCount();
            maxCountB->setValue(rc);
            countW->addWidget(maxCountB);
        }

        countW->stackHorizontally(p);
        countW->fitContent();
        addWidget(countW);
    }

    for(int i = 0; i < 3; i++) {
        const auto resourceTypeButton = new eResourceButton(window());
        resourceTypeButton->initialize([i, e](const eResourceType r){
            e->setResourceType(i, r);
        }, eResourceType::allBasic, true, true);
        const auto rr = e->resourceType(i);
        resourceTypeButton->setResource(rr);
        addWidget(resourceTypeButton);
    }

    stackVertically(p);
    fitContent();
}
