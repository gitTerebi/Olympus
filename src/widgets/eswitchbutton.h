#ifndef ESWITCHBUTTON_H
#define ESWITCHBUTTON_H

#include "framed-button.h"

class eSwitchButton : public FramedButton {
public:
    eSwitchButton(eMainWindow* const window);

    int addValue(const std::string& v);
    int currentValue() const { return mCurrentValue; }

    void switchValue();
    void switchValueBack();
    void setValue(const int v);

    using eSwitchAction = std::function<void(int)>;
    void setSwitchAction(const eSwitchAction& a);

    void fitValidContent();
private:
    int mCurrentValue = 0;
    std::vector<std::string> mValues;

    eSwitchAction mSwitchAction;
};

#endif // ESWITCHBUTTON_H
