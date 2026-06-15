#ifndef EBASICBUTTON_H
#define EBASICBUTTON_H

#include "ebutton.h"

#include "textures/game-textures.h"

class eBasicButton : public eButton {
public:
    using eTex = eTextureCollection InterfaceTextures::*;
    eBasicButton(const eTex tex,
                 eMainWindow* const window);
};

#endif // EBASICBUTTON_H
