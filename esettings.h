#ifndef ESETTINGS_H
#define ESETTINGS_H

#include "widgets/eresolution.h"
#include "engine/etile.h"

struct eSettings {
    static constexpr int sMinKeyScrollSpeed = 10;
    static constexpr int sMaxKeyScrollSpeed = 200;
    static constexpr int sMinVolume = 0;
    static constexpr int sMaxVolume = 100;

    bool fTinyTextures = true;
    bool fSmallTextures = true;
    bool fMediumTextures = true;
    bool fLargeTextures = true;
    bool fFullscreen = false;
    int fKeyScrollSpeed = 18;
    int fMusicVolume = 100;
    int fSoundsVolume = 100;
    eResolution fRes = eResolution(1280, 720);

    std::vector<eTileSize> availableSizes() const;
    static int clampKeyScrollSpeed(const int speed);
    static int clampVolume(const int volume);

    void write() const;
    void read();
};

#endif // ESETTINGS_H
