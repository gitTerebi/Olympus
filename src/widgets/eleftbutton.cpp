#include "eleftbutton.h"

#include "textures/interface-textures.h"

eLeftButton::eLeftButton(MainWindow* const window) :
    eBasicButton(&InterfaceTextures::fLeftButton, window) {}
