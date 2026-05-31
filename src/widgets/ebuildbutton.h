#ifndef EBUILDBUTTON_H
#define EBUILDBUTTON_H

#include "eflatbutton.h"

class eBuildButton : public eFlatButton {
public:
    using eFlatButton::eFlatButton;

    void initialize(const std::string& name,
                    const int marbleCost,
                    const int cost,
                    const int width = 0);
};

#endif // EBUILDBUTTON_H
