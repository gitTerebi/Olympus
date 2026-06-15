#include "ecancelbutton.h"

#include "textures/interface-textures.h"

eCancelButton::eCancelButton(MainWindow* const window) :
    eBasicButton(&InterfaceTextures::fCancelButton, window) {}
