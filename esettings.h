#ifndef ESETTINGS_H
#define ESETTINGS_H

#include "widgets/eresolution.h"
#include "engine/etile.h"

#include <SDL2/SDL_scancode.h>

enum class eHotkeyId {
    gameMenu,
    speedUp,
    speedDown,
    pause,
    rotatePreview,
    copyBuilding,
    deleteTool,
    showRoadsOverlay,
    buildRoad,
    buildRoadblock,
    buildMaintenanceOffice,
    scrollLeft,
    scrollRight,
    scrollUp,
    scrollDown,
    bookmark1,
    bookmark2,
    bookmark3,
    bookmark4
};

struct eSettings {
    static constexpr int sMinKeyScrollSpeed = 10;
    static constexpr int sMaxKeyScrollSpeed = 200;
    static constexpr int sMinVolume = 0;
    static constexpr int sMaxVolume = 100;
    static constexpr int sMinGameSpeed = 100;
    static constexpr int sMaxGameSpeed = 200;
    static constexpr int sDefaultGameSpeed = 100;

    bool fTinyTextures = true;
    bool fSmallTextures = true;
    bool fMediumTextures = true;
    bool fLargeTextures = true;
    bool fFullscreen = false;
    bool fWarehouseDefaultAcceptNone = false;
    bool fDisableEdgeScroll = false;
    int fKeyScrollSpeed = 18;
    int fGameSpeed = sDefaultGameSpeed;
    int fGeneralVolume = 100;
    int fMusicVolume = 100;
    int fVoiceVolume = 100;
    int fEventVolume = 100;
    int fAmbientVolume = 100;
    eResolution fRes = eResolution(1280, 720);

    SDL_Scancode fHotkeyGameMenu = SDL_SCANCODE_ESCAPE;
    SDL_Scancode fHotkeySpeedUp = SDL_SCANCODE_RIGHTBRACKET;
    SDL_Scancode fHotkeySpeedDown = SDL_SCANCODE_LEFTBRACKET;
    SDL_Scancode fHotkeyPause = SDL_SCANCODE_P;
    SDL_Scancode fHotkeyRotatePreview = SDL_SCANCODE_TAB;
    SDL_Scancode fHotkeyCopyBuilding = SDL_SCANCODE_C;
    SDL_Scancode fHotkeyDeleteTool = SDL_SCANCODE_Z;
    SDL_Scancode fHotkeyShowRoadsOverlay = SDL_SCANCODE_V;
    SDL_Scancode fHotkeyBuildRoad = SDL_SCANCODE_R;
    SDL_Scancode fHotkeyBuildRoadblock = SDL_SCANCODE_F;
    SDL_Scancode fHotkeyBuildMaintenanceOffice = SDL_SCANCODE_Q;
    SDL_Scancode fHotkeyScrollLeft = SDL_SCANCODE_A;
    SDL_Scancode fHotkeyScrollRight = SDL_SCANCODE_D;
    SDL_Scancode fHotkeyScrollUp = SDL_SCANCODE_W;
    SDL_Scancode fHotkeyScrollDown = SDL_SCANCODE_S;
    SDL_Scancode fHotkeyBookmark1 = SDL_SCANCODE_F1;
    SDL_Scancode fHotkeyBookmark2 = SDL_SCANCODE_F2;
    SDL_Scancode fHotkeyBookmark3 = SDL_SCANCODE_F3;
    SDL_Scancode fHotkeyBookmark4 = SDL_SCANCODE_F4;

    std::vector<eTileSize> availableSizes() const;
    static int clampKeyScrollSpeed(const int speed);
    static int clampVolume(const int volume);
    static int clampGameSpeed(const int speed);
    SDL_Scancode hotkey(const eHotkeyId id) const;
    void setHotkey(const eHotkeyId id, const SDL_Scancode key);

    void write() const;
    void read();
};

#endif // ESETTINGS_H
