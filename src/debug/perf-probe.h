#ifndef PERF_PROBE_H
#define PERF_PROBE_H

#include <chrono>
#include <cstdio>
#include <map>
#include <string>

// dev-only frame/sim timing probe, active with --dev-perf
namespace PerfProbe {

enum Section {
    FrameDt = 0,
    IncTime,
    GmUpdate,
    BoardMisc,     // planned actions, banners, disasters
    BoardCities,
    BoardPlayers,  // players, date/events, employment
    BoardChars,
    BoardSoldiers,
    BoardBuildings,
    BoardRest,     // spawners, missiles, puffs, goals, employees
    Paint,         // mWidget->paint()
    Present,       // SDL_RenderPresent
    FrameSleep,    // fps-cap sleep
    PaintTiles,    // iterateOverVisibleTiles lambda submission
    PaintFlush,    // tp.handleScheduledDraw
    PaintPost,     // world tex blit + postprocess shader
    TileTerr,      // terrain + overlays (fish, appeal, road preview)
    TileChars,     // character draw passes
    TileBuild,     // buildingDrawer + banners + crosswalk + missiles
    SectionCount
};

inline bool sEnabled = false;

// per-key timing (e.g. building type) for hotspot breakdown
struct KeyStat { double fMs = 0.0; long long fCalls = 0; };
inline std::map<int, KeyStat> sKeyMs;

inline void addKey(const int key, const double ms) {
    if(!sEnabled) return;
    auto& s = sKeyMs[key];
    s.fMs += ms;
    s.fCalls++;
}

inline double sMs[SectionCount] = {};
inline long long sFrames = 0;
inline long long sSimTicks = 0;
inline long long sTiles = 0; // tiles visited by the paint loop

inline void reset() {
    for(int i = 0; i < SectionCount; i++) sMs[i] = 0.0;
    sFrames = 0;
    sSimTicks = 0;
    sTiles = 0;
    sKeyMs.clear();
}

inline void add(const Section s, const double ms) {
    if(sEnabled) sMs[s] += ms;
}

using Clock = std::chrono::steady_clock;

inline double msSince(const Clock::time_point t0) {
    using ms_t = std::chrono::duration<double, std::milli>;
    return std::chrono::duration_cast<ms_t>(Clock::now() - t0).count();
}

// marks laps between sequential sections of one function
struct Lap {
    Clock::time_point fPrev;
    Lap() { if(sEnabled) fPrev = Clock::now(); }
    void lap(const Section s) {
        if(!sEnabled) return;
        const auto now = Clock::now();
        using ms_t = std::chrono::duration<double, std::milli>;
        sMs[s] += std::chrono::duration_cast<ms_t>(now - fPrev).count();
        fPrev = now;
    }
};

inline std::string report(const char* title) {
    if(sFrames <= 0) return std::string(title) + ": no frames\n";
    const double f = double(sFrames);
    char buf[1024];
    const double frameMs = sMs[FrameDt] / f;
    snprintf(buf, sizeof(buf),
        "%s: frames=%lld simTicks=%lld\n"
        "  frame       %8.3f ms/frame (%.1f fps)\n"
        "  incTime     %8.3f ms/frame\n"
        "    misc      %8.3f\n"
        "    cities    %8.3f\n"
        "    players   %8.3f\n"
        "    chars     %8.3f\n"
        "    soldiers  %8.3f\n"
        "    buildings %8.3f\n"
        "    rest      %8.3f\n"
        "  gmUpdate    %8.3f ms/frame\n"
        "  paint       %8.3f ms/frame\n"
        "    tiles     %8.3f (%lld tiles/frame)\n"
        "      terr    %8.3f\n"
        "      chars   %8.3f\n"
        "      build   %8.3f\n"
        "    flush     %8.3f\n"
        "    post      %8.3f\n"
        "  present     %8.3f ms/frame\n"
        "  capSleep    %8.3f ms/frame\n",
        title, sFrames, sSimTicks,
        frameMs, frameMs > 0.0 ? 1000.0 / frameMs : 0.0,
        sMs[IncTime] / f,
        sMs[BoardMisc] / f,
        sMs[BoardCities] / f,
        sMs[BoardPlayers] / f,
        sMs[BoardChars] / f,
        sMs[BoardSoldiers] / f,
        sMs[BoardBuildings] / f,
        sMs[BoardRest] / f,
        sMs[GmUpdate] / f,
        sMs[Paint] / f,
        sMs[PaintTiles] / f, sTiles / sFrames,
        sMs[TileTerr] / f,
        sMs[TileChars] / f,
        sMs[TileBuild] / f,
        sMs[PaintFlush] / f,
        sMs[PaintPost] / f,
        sMs[Present] / f,
        sMs[FrameSleep] / f);
    return buf;
}

} // namespace PerfProbe

#endif // PERF_PROBE_H
