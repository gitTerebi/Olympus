#include "erightbutton.h"

#include "textures/einterfacetextures.h"

eRightButton::eRightButton(eMainWindow* const window) :
    eBasicButton(&eInterfaceTextures::fRightButton, window) {}
