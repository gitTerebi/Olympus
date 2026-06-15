#include "eacceptbutton.h"

#include "textures/interface-textures.h"

eAcceptButton::eAcceptButton(MainWindow* const window) :
    eBasicButton(&InterfaceTextures::fAcceptButton, window) {}
