#include "eokbutton.h"

#include "textures/game-textures.h"

eOkButton::eOkButton(MainWindow* const window) :
    eBasicButton(&InterfaceTextures::fOkButton, window) {}
