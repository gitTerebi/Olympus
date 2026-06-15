#include "ehelpbutton.h"

#include "textures/game-textures.h"

eHelpButton::eHelpButton(eMainWindow* const window) :
    eBasicButton(&InterfaceTextures::fHelpButton, window) {}
