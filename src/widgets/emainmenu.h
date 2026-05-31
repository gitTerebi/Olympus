#ifndef EMAINMENU_H
#define EMAINMENU_H

#include "emainmenubase.h"

class eMainMenu : public eMainMenuBase {
public:
    using eMainMenuBase::eMainMenuBase;

    void initialize(const eAction& continueGameA,
                    const bool continueGameEnabled,
                    const eAction& newGameA,
                    const eAction& loadGameA,
                    const eAction& editGameA,
                    const eAction& settingsA,
                    const eAction& optionsA,
                    const eAction& quitA,
                    const eAction& leaderA);
};

#endif // EMAINMENU_H
