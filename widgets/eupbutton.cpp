#include "eupbutton.h"

#include "textures/einterfacetextures.h"

eUpButton::eUpButton(eMainWindow* const window) :
    eBasicButton(&eInterfaceTextures::fUpButton, window) {}
