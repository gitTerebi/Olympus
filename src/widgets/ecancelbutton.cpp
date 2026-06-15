#include "ecancelbutton.h"

#include "textures/interface-textures.h"

eCancelButton::eCancelButton(eMainWindow* const window) :
    eBasicButton(&InterfaceTextures::fCancelButton, window) {}
