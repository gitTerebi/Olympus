#include "ereceiverequesteventwidget.h"

#include "widgets/eresourcebutton.h"
#include "widgets/ecitybutton.h"
#include "widgets/evaluebutton.h"
#include "widgets/elabeledwidget.h"
#include "widgets/egodbutton.h"
#include "widgets/echoosebutton.h"
#include "emainwindow.h"
#include "elanguage.h"

void eReceiveRequestEventWidget::initialize(eReceiveRequestEvent* const e) {
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

    const auto godB = new eGodButton(window());

    {
        const auto typeB = new eFramedButton(window());
        typeB->setUnderline(false);
        typeB->setPressAction([this, typeB, godB, e]() {
            const auto choose = new eChooseButton(window());
            const std::vector<eReceiveRequestType> types {
                eReceiveRequestType::general,
                eReceiveRequestType::festival,
                eReceiveRequestType::project,
                eReceiveRequestType::famine,
                eReceiveRequestType::financialWoes
            };
            const std::vector<std::string> typeNames {
                eLanguage::zeusText(290, 1),
                eLanguage::zeusText(290, 2),
                eLanguage::zeusText(290, 3),
                eLanguage::zeusText(290, 4),
                eLanguage::zeusText(290, 5)
            };

            const auto act = [typeB, godB, e, types, typeNames](const int val) {
                const auto type = types[val];
                godB->setVisible(type == eReceiveRequestType::festival);
                e->setRequestType(type);
                const auto& name = typeNames[val];
                typeB->setText(name);
            };
            choose->initialize(7, typeNames, act);

            window()->execDialog(choose);
            choose->align(eAlignment::center);
        });

        const std::map<eReceiveRequestType, std::string> typeNames {
            {eReceiveRequestType::general, eLanguage::zeusText(290, 1)},
            {eReceiveRequestType::festival, eLanguage::zeusText(290, 2)},
            {eReceiveRequestType::project, eLanguage::zeusText(290, 3)},
            {eReceiveRequestType::famine, eLanguage::zeusText(290, 4)},
            {eReceiveRequestType::financialWoes, eLanguage::zeusText(290, 5)}
        };
        const auto type = e->requestType();
        godB->setVisible(type == eReceiveRequestType::festival);
        typeB->setText(typeNames.at(type));
        typeB->fitContent();
        addWidget(typeB);
    }
    {
        const auto countW = new eWidget(window());
        countW->setNoPadding();
        {
            const auto minCountB = new eValueButton(window());
            minCountB->setValueChangeAction([e](const int p) {
                e->setMinCount(p);
            });
            minCountB->initialize(1, 999);
            const int rc = e->minCount();
            minCountB->setValue(rc);
            countW->addWidget(minCountB);
        }
        {
            const auto maxCountB = new eValueButton(window());
            maxCountB->setValueChangeAction([e](const int p) {
                e->setMaxCount(p);
            });
            maxCountB->initialize(1, 999);
            const int rc = e->maxCount();
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

    godB->initialize([e](const eGodType type) {
        e->setGod(type);
    });
    const auto g = e->god();
    godB->setType(g);
    addWidget(godB);

    stackVertically(p);
    fitContent();
}
