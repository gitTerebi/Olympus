#ifndef EOVERLAY_H
#define EOVERLAY_H

#include "etexture.h"

struct eOverlay {
    double fX;
    double fY;
    std::shared_ptr<eTexture> fTex;
    bool fAlignTop = false;
    bool fOnTop = false; // draw after characters (e.g. pier dockworker over docked boat)
};

#endif // EOVERLAY_H
