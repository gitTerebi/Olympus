#include "eacceptbutton.h"

#include "textures/einterfacetextures.h"

eAcceptButton::eAcceptButton(eMainWindow* const window) :
    eBasicButton(&eInterfaceTextures::fAcceptButton, window) {}
