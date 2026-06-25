#include "dev-launch.h"

#include "engine/eworlddirection.h"
#include "main-window.h"
#include "widgets/game-widget.h"
#include "widgets/erosterofleaders.h"

#include <chrono>
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

}

void applyDevLaunchOptions(MainWindow& window,
                           const DevLaunchOptions& options)
{
    if(!options.fLoadRecent && !options.fCycleDirs) return;
    window.addSlot([&window, options]() {
        if(options.fLoadRecent) {
            const bool loaded = loadMostRecentGame(window);
            if(!loaded) printf("dev-load-recent: no recent save loaded\n");
        }
        if(options.fCycleDirs) {
            startDirectionCycle(window);
        }
    });
}
