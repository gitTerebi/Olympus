#include "ecancelbutton.h"

#include "textures/einterfacetextures.h"

eCancelButton::eCancelButton(eMainWindow* const window) :
    eBasicButton(&eInterfaceTextures::fCancelButton, window) {}
