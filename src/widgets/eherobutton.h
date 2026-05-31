#ifndef EHEROBUTTON_H
#define EHEROBUTTON_H

#include "framed-button.h"

#include "characters/heroes/ehero.h"

class GameBoard;

class eHeroButton : public FramedButton {
public:
    using FramedButton::FramedButton;

    using eHeroAction = std::function<void(const eHeroType)>;
    void initialize(const eHeroAction& hact);

    eHeroType type() const { return mType; }
    void setType(const eHeroType type);
private:
    eHeroType mType;
};

#endif // EHEROBUTTON_H
