#include "dev-launch.h"

#include "engine/eworlddirection.h"
#include "language.h"
#include "main-window.h"
#include "widgets/game-widget.h"
#include "widgets/erosterofleaders.h"
#include "widgets/eworldwidget.h"

#include <SDL2/SDL_image.h>

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <memory>

namespace {

std::string mostRecentSavePath(const MainWindow& window)
{
    const auto folder = window.leaderSaveDir();
    if(!std::filesystem::exists(folder)) return "";

    bool found = false;
    std::filesystem::file_time_type bestTime;
    std::filesystem::path bestPath;
    for(const auto& entry : std::filesystem::directory_iterator(folder)) {
        const auto path = entry.path();
        if(path.extension() != ".ez2") continue;
        const auto time = std::filesystem::last_write_time(path);
        if(!found || time > bestTime) {
            found = true;
            bestTime = time;
            bestPath = path;
        }
    }
    if(!found) return "";
    return bestPath.string();
}

bool loadMostRecentGame(MainWindow& window)
{
    const auto leaders = eRosterOfLeaders::sLeaders();
    if(window.leader().empty() && leaders.size() == 1) {
        window.setLeader(leaders[0]);
    }
    if(window.leader().empty()) return false;

    const auto path = mostRecentSavePath(window);
    if(path.empty()) return false;
    return window.loadGame(path);
}

const char* directionName(const eWorldDirection dir)
{
    if(dir == eWorldDirection::W) return "W";
    if(dir == eWorldDirection::S) return "S";
    if(dir == eWorldDirection::E) return "E";
    return "N";
}

void showWorldWhenReady(MainWindow& window)
{
    const auto tick = std::make_shared<eSlot>();
    const auto done = std::make_shared<bool>(false);
    *tick = [&window, tick, done]() {
        if(*done) return;
        const auto gameWidget =
            dynamic_cast<GameWidget*>(window.currentWidget());
        if(gameWidget) {
            *done = true;
            window.showWorld();
            return;
        }
        window.addSlot(*tick);
    };
    window.addSlot(*tick);
}

void startDirectionCycle(MainWindow& window)
{
    struct CycleState {
        int fDir = -1;
        std::chrono::steady_clock::time_point fLast{};
    };
    const auto state = std::make_shared<CycleState>();
    const auto tick = std::make_shared<eSlot>();
    *tick = [&window, state, tick]() {
        const auto now = std::chrono::steady_clock::now();
        const bool first = state->fDir == -1;
        const bool elapsed =
            !first &&
            std::chrono::duration_cast<std::chrono::milliseconds>(
                now - state->fLast).count() >= 2000;
        const auto gameWidget =
            dynamic_cast<GameWidget*>(window.currentWidget());
        if(gameWidget && (first || elapsed)) {
            state->fDir = (state->fDir + 1) % 4;
            state->fLast = now;
            const auto dir = static_cast<eWorldDirection>(state->fDir);
            printf("dev-cycle-dirs: %s\n", directionName(dir));
            gameWidget->setWorldDirection(dir);
        }
        window.addSlot(*tick);
    };
    window.addSlot(*tick);
}

bool saveRendererShot(MainWindow& window, const std::string& path)
{
    if(path.empty()) return false;
    const int w = window.width();
    const int h = window.height();
    if(w <= 0 || h <= 0) return false;

    auto surface = SDL_CreateRGBSurfaceWithFormat(
        0, w, h, 32, SDL_PIXELFORMAT_ARGB8888);
    if(!surface) {
        printf("dev-world-map-shot: create surface failed: %s\n",
               SDL_GetError());
        return false;
    }

    const int read = SDL_RenderReadPixels(window.renderer(), nullptr,
                                          SDL_PIXELFORMAT_ARGB8888,
                                          surface->pixels, surface->pitch);
    if(read != 0) {
        printf("dev-world-map-shot: read pixels failed: %s\n",
               SDL_GetError());
        SDL_FreeSurface(surface);
        return false;
    }

    const auto fsp = std::filesystem::path(path);
    const auto parent = fsp.parent_path();
    if(!parent.empty()) std::filesystem::create_directories(parent);
    const int saved = IMG_SavePNG(surface, path.c_str());
    SDL_FreeSurface(surface);
    if(saved != 0) {
        printf("dev-world-map-shot: save '%s' failed: %s\n",
               path.c_str(), IMG_GetError());
        return false;
    }
    printf("dev-world-map-shot: saved %s (%dx%d)\n", path.c_str(), w, h);
    return true;
}

void saveWorldShotWhenReady(MainWindow& window, const std::string& path,
                            const std::string& clickCity)
{
    struct ShotState {
        int fFramesAfterWorld = 0;
        bool fClicked = false;
    };
    const auto state = std::make_shared<ShotState>();
    const auto tick = std::make_shared<eSlot>();
    *tick = [&window, path, clickCity, state, tick]() {
        const auto ww = window.worldWidget();
        if(ww) {
            if(!state->fClicked && !clickCity.empty()) {
                state->fClicked = ww->selectCityByName(clickCity);
                if(!state->fClicked) {
                    printf("dev-click-city: '%s' not found\n",
                           clickCity.c_str());
                    state->fClicked = true; // do not retry forever
                }
            }
            state->fFramesAfterWorld++;
        }
        if(state->fFramesAfterWorld >= 3) {
            saveRendererShot(window, path);
            window.quit();
            return;
        }
        window.addSlot(*tick);
    };
    window.addSlot(*tick);
}

}

void applyDevLaunchOptions(MainWindow& window,
                           const DevLaunchOptions& options)
{
    if(!options.fLoadRecent && !options.fCycleDirs &&
       !options.fWorldMap) return;
    const auto launch = std::make_shared<eSlot>();
    *launch = [&window, options, launch]() {
        if(!Language::loaded()) {
            window.addSlot(*launch);
            return;
        }
        const bool wantLoad = options.fLoadRecent || options.fWorldMap;
        bool loaded = false;
        if(wantLoad) {
            loaded = loadMostRecentGame(window);
            if(!loaded) printf("dev-load-recent: no recent save loaded\n");
        }
        if(options.fWorldMap && loaded) {
            showWorldWhenReady(window);
        }
        if(!options.fWorldMapShotPath.empty()) {
            saveWorldShotWhenReady(window, options.fWorldMapShotPath,
                                   options.fClickCity);
        }
        if(options.fCycleDirs) {
            startDirectionCycle(window);
        }
    };
    window.addSlot(*launch);
}
