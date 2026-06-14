#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <string>

#include <filesystem>

#include "emainwindow.h"
#include "textures/egametextures.h"

#include "egamedir.h"
#include "engine/model-data.h"
#include "enumbers.h"

#include "audio/music.h"
#include "audio/sounds.h"
#include "audio/audio-device.h"
#include "cursors.h"
#include "debug/windows-dump.h"

bool init() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n",
               SDL_GetError());
        return false;
    }

    if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        printf("SDL audio subsystem could not initialize! SDL Error: %s\n",
               SDL_GetError());
    }

    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0")) {
        printf("Warning: Nearest texture filtering not enabled!");
    }

    // The whole-frame upscale is a D3D11/HLSL pass, so force the d3d11 renderer.
    if(!SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d11")) {
        printf("Warning: could not request Direct3D 11 render driver!\n");
    }

    const int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n",
               IMG_GetError());
        return false;
    }

    const int mixFlags = MIX_INIT_MP3 | MIX_INIT_OGG;
    if((Mix_Init(mixFlags) & mixFlags) != mixFlags) {
        printf("SDL_mixer codec init incomplete! Mix Error: %s\n",
               Mix_GetError());
    }

    if(!ensureAudioDeviceOpen()) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n",
               Mix_GetError());
    }

    if(TTF_Init()) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n",
               TTF_GetError());
        return false;
    }

    return true;
}

void close() {
    TTF_Quit();
    IMG_Quit();
    Mix_Quit();
    SDL_Quit();
}

bool getDisplayResolution(SDL_DisplayMode& mode) {
    int display_count = 0, display_index = 0, mode_index = 0;
    mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };

    if((display_count = SDL_GetNumVideoDisplays()) < 1) {
        SDL_Log("SDL_GetNumVideoDisplays returned: %i", display_count);
        return false;
    }

    if(SDL_GetDisplayMode(display_index, mode_index, &mode) != 0) {
        SDL_Log("SDL_GetDisplayMode failed: %s", SDL_GetError());
        return false;
    }
    SDL_Log("SDL_GetDisplayMode(0, 0, &mode):\t\t%i bpp\t%i x %i",
    SDL_BITSPERPIXEL(mode.format), mode.w, mode.h);

    return true;
}

bool getDisplayResolutions(std::vector<SDL_DisplayMode>& resolutions) {
    const int display_count = SDL_GetNumVideoDisplays();

    SDL_Log("Number of displays: %i", display_count);

    for(int display_index = 0; display_index <= display_count; display_index++) {
        SDL_Log("Display %i:", display_index);

        const int modes_count = SDL_GetNumDisplayModes(display_index);

        for(int mode_index = 0; mode_index <= modes_count; mode_index++) {
            SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };

            if (SDL_GetDisplayMode(display_index, mode_index, &mode) == 0) {
                SDL_Log(" %i bpp\t%i x %i @ %iHz",
                    SDL_BITSPERPIXEL(mode.format), mode.w, mode.h, mode.refresh_rate);

                resolutions.push_back(mode);
            }
        }
    }
    return true;
}

int main() {
    installWindowsDumpHandler();

    if(!init()) {
        printf("Failed to initialize!\n");
        close();
        return 1;
    }
    installWindowsDumpHandler();

    eGameDir::initialize();
    ModelData::instance().load();

//    SDL_DisplayMode mode;
//    const bool r0 = getDisplayResolution(mode);
//    eResolution resolution;
//    if(r0) {
//        resolution = eResolution(mode.w, mode.h);
//    } else {
//        resolution = eResolution(1280, 720);
//    }

//    std::vector<SDL_DisplayMode> resolutions;
//    const bool r1 = getDisplayResolutions(resolutions);
//    if(r1 && !resolutions.empty()) {
//        eResolution::sResolutions = std::vector<eResolution>();
//        for(const auto& m : resolutions) {
//            eResolution::sResolutions.emplace_back(m.w, m.h);
//        }
//    }
    std::vector<SDL_DisplayMode> ress;
    const int displayCount = SDL_GetNumVideoDisplays();

    for(int i = 0; i < displayCount; i++) {
        SDL_DisplayMode dm;
        SDL_GetCurrentDisplayMode(i, &dm);
        bool contains = false;
        for(const auto& rr : eResolution::sResolutions) {
            contains = dm.w == rr.width() &&
                       dm.h == rr.height();
            if(contains) break;
        }
        if(!contains) {
            const auto res = eResolution(dm.w, dm.h);
            eResolution::sResolutions.push_back(res);
        }
    }

    if(!std::filesystem::exists(eGameDir::iBinaryPath())) {
        printf("Could not find interface textures!\n"
               "Make sure you have interface.e file in eZeus directory.\n");
        close();
        return 1;
    }

    eNumbers::sLoad();
    eSettings settings;
    settings.read();
    bool found = false;
    const auto checkTextureSize = [&found](const std::string& path,
                                           bool& setting) {
        if(!setting) return;
        setting = std::filesystem::exists(path);
        if(setting) found = true;
    };
    checkTextureSize(eGameDir::i15BinaryPath(), settings.fTinyTextures);
    checkTextureSize(eGameDir::i30BinaryPath(), settings.fSmallTextures);
    checkTextureSize(eGameDir::i45BinaryPath(), settings.fMediumTextures);
    checkTextureSize(eGameDir::i60BinaryPath(), settings.fLargeTextures);
    if(!found) {
        settings.fSmallTextures = std::filesystem::exists(eGameDir::i30BinaryPath());
        if(settings.fSmallTextures) {
            found = true;
        } else {
            printf("Could not find any textures!\n"
                   "Make sure you have i15.e, i30.e, i45.e, or i60.e file in eZeus directory.\n");
            close();
            return 1;
        }
    }

    int r = 0;
    {
        eMusic music;
        eSounds sounds;
        eMainWindow w;
        const bool i = w.initialize(settings);
        if(!i) return 1;
        const bool e = eGameTextures::initialize(w.renderer());
        Cursors::initialize();

        if(e) r = w.exec();
        Cursors::destroy();
    }

    close();

    return r;
}
