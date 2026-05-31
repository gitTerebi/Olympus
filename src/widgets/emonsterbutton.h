#ifndef EMONSTERBUTTON_H
#define EMONSTERBUTTON_H

#include "framed-button.h"

#include "characters/monsters/emonstertype.h"

class GameBoard;

class eMonsterButton : public FramedButton {
public:
    using FramedButton::FramedButton;

    using eMonsterAction = std::function<void(const eMonsterType)>;
    void initialize(const eMonsterAction& mact,
                    const bool withGodsOnly = false,
                    const bool showGodNames = true);

    eMonsterType type() const { return mType; }
    void setType(const eMonsterType type);
private:
    eMonsterType mType;
};

#endif // EMONSTERBUTTON_H
