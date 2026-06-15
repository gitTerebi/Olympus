#include "eokbutton.h"

#include "textures/game-textures.h"

eOkButton::eOkButton(eMainWindow* const window) :
    eBasicButton(&InterfaceTextures::fOkButton, window) {}
