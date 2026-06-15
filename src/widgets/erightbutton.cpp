#include "erightbutton.h"

#include "textures/interface-textures.h"

eRightButton::eRightButton(MainWindow* const window) :
    eBasicButton(&InterfaceTextures::fRightButton, window) {}
