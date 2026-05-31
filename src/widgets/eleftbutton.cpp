#include "eleftbutton.h"

#include "textures/einterfacetextures.h"

eLeftButton::eLeftButton(eMainWindow* const window) :
    eBasicButton(&eInterfaceTextures::fLeftButton, window) {}
