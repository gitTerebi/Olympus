#ifndef ESETTINGS_H
#define ESETTINGS_H

#include "widgets/eresolution.h"
#include "engine/etile.h"

struct eSettings {
    static constexpr int sMinKeyScrollSpeed = 10;
    static constexpr int sMaxKeyScrollSpeed = 200;

    bool fTinyTextures = true;
    bool fSmallTextures = true;
    bool fMediumTextures = true;
    bool fLargeTextures = true;
    bool fFullscreen = false;
    int fKeyScrollSpeed = 18;
    eResolution fRes = eResolution(1280, 720);

    std::vector<eTileSize> availableSizes() const;
    static int clampKeyScrollSpeed(const int speed);

    void write() const;
    void read();
};

#endif // ESETTINGS_H
