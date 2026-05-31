#ifndef ETYPEBUTTON_H
#define ETYPEBUTTON_H

#include "eframedbutton.h"

class eTypeButton : public eFramedButton {
public:
    using eFramedButton::eFramedButton;

    void initialize(const int value,
                    const std::vector<std::string>& values,
                    const std::function<void(int)>& action);
};

#endif // ETYPEBUTTON_H
