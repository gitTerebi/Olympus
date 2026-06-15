#include "eacceptbutton.h"

#include "textures/interface-textures.h"

eAcceptButton::eAcceptButton(eMainWindow* const window) :
    eBasicButton(&InterfaceTextures::fAcceptButton, window) {}
