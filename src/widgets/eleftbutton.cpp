#include "eleftbutton.h"

#include "textures/interface-textures.h"

eLeftButton::eLeftButton(eMainWindow* const window) :
    eBasicButton(&InterfaceTextures::fLeftButton, window) {}
