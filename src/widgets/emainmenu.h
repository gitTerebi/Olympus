#ifndef EMAINMENU_H
#define EMAINMENU_H

#include "emainmenubase.h"

class FramedButton;

class eMainMenu : public eMainMenuBase {
public:
    using eMainMenuBase::eMainMenuBase;

    void initialize(const eAction& continueGameA,
                    const bool continueGameEnabled,
                    const eAction& newGameA,
                    const eAction& loadGameA,
                    const eAction& editGameA,
                    const eAction& optionsA,
                    const eAction& quitA,
                    const eAction& leaderA);
protected:
    void paintEvent(ePainter& p) override;
    void windowSizeChanged(int w, int h) override;
private:
    void layoutControls();

    eWidget* mButtons = nullptr;
    FramedButton* mLeader = nullptr;
};

#endif // EMAINMENU_H
