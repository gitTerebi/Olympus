#ifndef EVALUEBUTTON_H
#define EVALUEBUTTON_H

#include "framed-button.h"

class eValueButton : public FramedButton {
public:
    using FramedButton::FramedButton;

    void initialize(const int min, const int max);

    using eValueChangeAction = std::function<void(const int)>;
    void setValueChangeAction(const eValueChangeAction& a);

    void setValue(const int v);
    int value() const { return mValue; }
private:
    eValueChangeAction mValueChangeAction;
    int mValue;
};

#endif // EVALUEBUTTON_H
