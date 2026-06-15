#ifndef OVERLAY_H
#define OVERLAY_H

#include "texture.h"

struct Overlay {
    double fX;
    double fY;
    std::shared_ptr<Texture> fTex;
    bool fAlignTop = false;
    bool fOnTop = false; // draw after characters (e.g. pier dockworker over docked boat)
};

#endif // OVERLAY_H
