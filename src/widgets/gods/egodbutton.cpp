#include "egodbutton.h"

#include "widgets/echoosebutton.h"
#include "main-window.h"

void eGodButton::initialize(const eGodAction& gact) {
    setUnderline(false);
    setPressAction([this, gact]() {
        const auto choose = new eChooseButton(window());
        std::vector<GodType> gods;
        std::vector<std::string> godNames;
        God::sGodStrings(gods, godNames);
        const auto act = [this, gods, gact](const int val) {
            const auto t = gods[val];
            setType(t);
            if(gact) gact(t);
        };
        choose->initialize(7, godNames, act);

        window()->execDialog(choose);
        choose->align(Alignment::center);
    });
    setType(GodType::hephaestus);
    fitContent();
}

void eGodButton::setType(const GodType type) {
    setText(God::sGodName(type));
    mType = type;
}
