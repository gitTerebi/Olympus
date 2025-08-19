#include "ecitybecomeseventwidget.h"

#include "widgets/elabeledwidget.h"
#include "widgets/ecitybutton.h"
#include "elanguage.h"

#include "widgets/echoosebutton.h"
#include "emainwindow.h"

#include "gameEvents/ecitybecomesevent.h"

void eCityBecomesEventWidget::initialize(eCityBecomesEvent* const e) {
    const auto typeButtonL = new eLabeledWidget(window());
    const auto typeButton = new eFramedButton(window());
    const std::vector<eCityBecomesType> types{eCityBecomesType::ally,
                                              eCityBecomesType::rival,
                                              eCityBecomesType::vassal,

                                              eCityBecomesType::active,
                                              eCityBecomesType::inactive,

                                              eCityBecomesType::visible,
                                              eCityBecomesType::invisible};
    const std::vector<std::string> typeNames{eLanguage::zeusText(253, 0),
                                             eLanguage::zeusText(253, 1),
                                             eLanguage::zeusText(253, 2),

                                             eLanguage::zeusText(44, 248),
                                             eLanguage::zeusText(44, 249),

                                             eLanguage::zeusText(44, 307),
                                             eLanguage::zeusText(44, 306)};
    const auto type = e->type();
    const auto typeStr = typeNames[static_cast<int>(type)];
    typeButton->setText(typeStr);
    typeButton->fitContent();
    typeButton->setPressAction([this, e, types, typeNames, typeButton] {
        const auto chooseButton = new eChooseButton(window());
        const auto act = [e, types, typeNames, typeButton](const int val) {
            const auto t = types[val];
            e->setType(t);
            const auto typeStr = typeNames[static_cast<int>(t)];
            typeButton->setText(typeStr);
        };
        chooseButton->initialize(3, typeNames, act);
        window()->execDialog(chooseButton);
        chooseButton->align(eAlignment::center);
    });
    typeButtonL->setup(eLanguage::zeusText(44, 358), typeButton);
    addWidget(typeButtonL);

    const auto cityButtonL = new eLabeledWidget(window());
    const auto cc = e->city();
    const auto cityButton = new eCityButton(window());
    const auto board = e->worldBoard();
    cityButton->initialize(board, [e](const stdsptr<eWorldCity>& c){
        e->setCity(c);
    });
    cityButton->setCity(cc);
    cityButtonL->setup(eLanguage::zeusText(44, 271), cityButton);
    addWidget(cityButtonL);

    const int p = padding();
    stackVertically(p);
    fitContent();
}
