#include "etypebutton.h"

#include "widgets/echoosebutton.h"
#include "emainwindow.h"

void eTypeButton::initialize(const int value,
                             const std::vector<std::string>& values,
                             const std::function<void (int)>& action) {
    setUnderline(false);
    setText(values[value]);

    fitOptions(values);

    setPressAction([this, values, action] {
        const auto chooseButton = new eChooseButton(window());
        const auto act = [this, values, action](const int val) {
            if(action) action(val);
            const auto str = values[val];
            setText(str);
        };
        chooseButton->initialize(values.size()/3, values, act);
        window()->execDialog(chooseButton);
        chooseButton->align(eAlignment::center);
    });
}
