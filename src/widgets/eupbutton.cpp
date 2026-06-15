#include "eupbutton.h"

#include "textures/interface-textures.h"

eUpButton::eUpButton(MainWindow* const window) :
    eBasicButton(&InterfaceTextures::fUpButton, window) {}
