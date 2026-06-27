#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <algorithm>
#include <string>
#include <vector>

#include "dev-launch.h"
#include "main-window.h"
#include "textures/game-textures.h"

#include "game-dir.h"
#include "engine/model-data.h"
#include "numbers.h"

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

    // Prefer D3D11 on Windows. Other platforms use SDL's OpenGL renderer so the
    // postprocess path can use GLSL.
#ifdef _WIN32
    const char* const renderDriver = "direct3d11";
#else
    const char* const renderDriver = "opengl";
#endif
    if(!SDL_SetHint(SDL_HINT_RENDER_DRIVER, renderDriver)) {
        printf("Warning: could not request render driver '%s'!\n",
               renderDriver);
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

int main(int argc, char* argv[]) {
    installWindowsDumpHandler();
    std::vector<std::string> args;
    for(int i = 1; i < argc; i++) {
        args.emplace_back(argv[i]);
    }
    DevLaunchOptions devOptions;
    devOptions.fLoadRecent =
        std::find(args.begin(), args.end(), "--dev-load-recent") != args.end();
    devOptions.fCycleDirs =
        std::find(args.begin(), args.end(), "--dev-cycle-dirs") != args.end();
    devOptions.fWorldMap =
        std::find(args.begin(), args.end(), "--dev-world-map") != args.end();
    const auto worldMapShot =
        std::find(args.begin(), args.end(), "--dev-world-map-shot");
    if(worldMapShot != args.end()) {
        devOptions.fWorldMap = true;
        const auto pathArg = worldMapShot + 1;
        if(pathArg != args.end()) {
            devOptions.fWorldMapShotPath = *pathArg;
        }
    }

    if(!init()) {
        printf("Failed to initialize!\n");
        close();
        return 1;
    }
    installWindowsDumpHandler();

    GameDir::initialize();
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

    // Sprites decode live from DATA/*.sg3 + *.555, so the legacy interface.e
    // and i15/i30/i45/i60.e files are no longer required at startup. The zoom
    // flags below still pick which tier loads; emainwindow defaults one on if
    // none are set.
    Numbers::sLoad();
    Settings settings;
    settings.read();

    int r = 0;
    {
        eMusic music;
        eSounds sounds;
        MainWindow w;
        const bool offscreen = !devOptions.fWorldMapShotPath.empty();
        const bool i = w.initialize(settings, offscreen);
        if(!i) return 1;
        const bool e = GameTextures::initialize(w.renderer());
        Cursors::initialize();

        if(e) {
            applyDevLaunchOptions(w, devOptions);
            r = w.exec();
        }
        Cursors::destroy();
    }

    close();

    return r;
}
