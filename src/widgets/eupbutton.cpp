#include "eupbutton.h"

#include "textures/interface-textures.h"

eUpButton::eUpButton(eMainWindow* const window) :
    eBasicButton(&InterfaceTextures::fUpButton, window) {}
