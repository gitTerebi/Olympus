#ifndef ETYPEBUTTON_H
#define ETYPEBUTTON_H

#include "framed-button.h"

class eTypeButton : public FramedButton {
public:
    using FramedButton::FramedButton;

    void initialize(const int value,
                    const std::vector<std::string>& values,
                    const std::function<void(int)>& action);
};

#endif // ETYPEBUTTON_H
