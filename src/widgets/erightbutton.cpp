#include "erightbutton.h"

#include "textures/interface-textures.h"

eRightButton::eRightButton(eMainWindow* const window) :
    eBasicButton(&InterfaceTextures::fRightButton, window) {}
