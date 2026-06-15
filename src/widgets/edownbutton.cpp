#include "edownbutton.h"

#include "textures/game-textures.h"

eDownButton::eDownButton(eMainWindow* const window) :
    eBasicButton(&InterfaceTextures::fDownButton, window) {}
