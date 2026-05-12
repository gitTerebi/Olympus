#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <string>

#include <filesystem>
#include <cstdio>
#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#include <ctime>
#ifdef interface
#undef interface
#endif
#endif

#include "emainwindow.h"
#include "textures/egametextures.h"

#include "egamedir.h"
#include "enumbers.h"

#include "audio/emusic.h"
#include "audio/esounds.h"
#include "ecursors.h"

#ifdef _WIN32
static bool shouldWriteCrashDump(const DWORD code) {
    switch(code) {
    case EXCEPTION_ACCESS_VIOLATION:
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
    case EXCEPTION_DATATYPE_MISALIGNMENT:
    case EXCEPTION_FLT_DENORMAL_OPERAND:
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
    case EXCEPTION_FLT_INEXACT_RESULT:
    case EXCEPTION_FLT_INVALID_OPERATION:
    case EXCEPTION_FLT_OVERFLOW:
    case EXCEPTION_FLT_STACK_CHECK:
    case EXCEPTION_FLT_UNDERFLOW:
    case EXCEPTION_ILLEGAL_INSTRUCTION:
    case EXCEPTION_IN_PAGE_ERROR:
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
    case EXCEPTION_INT_OVERFLOW:
    case EXCEPTION_INVALID_DISPOSITION:
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
    case EXCEPTION_PRIV_INSTRUCTION:
    case EXCEPTION_STACK_OVERFLOW:
        return true;
    default:
        return false;
    }
}

static bool isSdlExceptionAddress(void* const address) {
    HMODULE module = nullptr;
    if(!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCSTR>(address), &module)) {
        return false;
    }

    char modulePath[MAX_PATH];
    const DWORD len = GetModuleFileNameA(module, modulePath, sizeof(modulePath));
    if(len == 0) return false;

    const std::filesystem::path path(modulePath);
    const auto name = path.filename().string();
    return name == "SDL2.dll";
}

static LONG WINAPI unhandledCrashHandler(EXCEPTION_POINTERS* info) {
    const auto code = info && info->ExceptionRecord ?
                      info->ExceptionRecord->ExceptionCode : 0;
    void* address = info && info->ExceptionRecord ?
                    info->ExceptionRecord->ExceptionAddress : nullptr;
    if(!shouldWriteCrashDump(code)) {
        printf("Unhandled non-crash exception code=0x%08lx address=%p; no dump\n",
               code, address);
        return EXCEPTION_EXECUTE_HANDLER;
    }
    if(isSdlExceptionAddress(address)) {
        printf("Unhandled SDL exception code=0x%08lx address=%p; no dump\n",
               code, address);
        return EXCEPTION_EXECUTE_HANDLER;
    }

    char dumpPath[MAX_PATH];
    const std::time_t now = std::time(nullptr);
    snprintf(dumpPath, sizeof(dumpPath),
             "C:/Users/somtam/Desktop/ezeus_crash_%lld.dmp",
             static_cast<long long>(now));

    HANDLE file = CreateFileA(dumpPath, GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if(file != INVALID_HANDLE_VALUE) {
        MINIDUMP_EXCEPTION_INFORMATION mei;
        mei.ThreadId = GetCurrentThreadId();
        mei.ExceptionPointers = info;
        mei.ClientPointers = FALSE;
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file,
                          MiniDumpWithDataSegs, &mei, nullptr, nullptr);
        CloseHandle(file);
    }

    printf("Unhandled crash code=0x%08lx address=%p dump=%s\n",
           code, address, dumpPath);
    return EXCEPTION_EXECUTE_HANDLER;
}

static void installCrashHandler() {
    SetUnhandledExceptionFilter(unhandledCrashHandler);
}
#else
static void installCrashHandler() {}
#endif

bool init() {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n",
               SDL_GetError());
        return false;
    }

    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
        printf("Warning: Linear texture filtering not enabled!");
    }

    const int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n",
               IMG_GetError());
        return false;
    }

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n",
               Mix_GetError());
        return false;
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
    installCrashHandler();
    if(!init()) {
        printf("Failed to initialize!\n");
        close();
        return 1;
    }

    eGameDir::initialize();

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
        printf("Could not find any textures!\n"
               "Make sure you have i15.e, i30.e, i45.e, or i60.e file in eZeus directory.\n");
        close();
        return 1;
    }

    int r = 0;
    {
        eMusic music;
        eSounds sounds;
        eMainWindow w;
        const bool i = w.initialize(settings);
        if(!i) return 1;
        const bool e = eGameTextures::initialize(w.renderer());
        eCursors::initialize();

        if(e) r = w.exec();
        eCursors::destroy();
    }

    close();

    return r;
}
