#ifndef EGODBUTTON_H
#define EGODBUTTON_H

#include "widgets/framed-button.h"

#include "characters/gods/god.h"

class GameBoard;

class eGodButton : public FramedButton {
public:
    using FramedButton::FramedButton;

    using eGodAction = std::function<void(const GodType)>;
    void initialize(const eGodAction& gact);

    GodType type() const { return mType; }
    void setType(const GodType type);
private:
    GodType mType;
};

#endif // EGODBUTTON_H
