#include "game-dir.h"

#include <SDL2/SDL_filesystem.h>
#include <cstdio>
#include <fstream>
#ifdef __unix__
#include <filesystem>
#include <algorithm>
#include <cctype>
#endif

std::string GameDir::sPath;

void GameDir::initialize() {
    sPath = exeDir() + "../../";
    const auto zp = exeDir() + "../zeus_path.txt";
    std::ifstream file(zp);
    if(!file.good()) {
        printf("Game data path: %s (default; no %s)\n",
               sPath.c_str(), zp.c_str());
        return;
    }
    std::string str;
    const bool g = !!std::getline(file, str);
    if(!g) {
        printf("Game data path: %s (default; empty %s)\n",
               sPath.c_str(), zp.c_str());
        return;
    }
    const bool absolute = !str.empty() &&
                          (str[0] == '/' || str[0] == '\\' ||
                           (str.size() > 1 && str[1] == ':'));
    sPath = absolute ? str : exeDir() + str;
    printf("Game data path: %s (from %s)\n", sPath.c_str(), zp.c_str());
}

#ifdef __unix__
// The game data was made for case-insensitive Windows, so match each path
// part ignoring case when the exact name does not exist.
static std::string sLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
}

static std::string sResolveCase(const std::string& base,
                                const std::string& rel) {
    namespace fs = std::filesystem;
    std::error_code ec;
    fs::path cur(base);
    size_t pos = 0;
    while(pos < rel.size()) {
        auto end = rel.find('/', pos);
        if(end == std::string::npos) end = rel.size();
        const std::string part = rel.substr(pos, end - pos);
        pos = end + 1;
        if(part.empty()) continue;
        fs::path next = cur / part;
        if(!fs::exists(next, ec)) {
            const auto lower = sLower(part);
            for(const auto& e : fs::directory_iterator(cur, ec)) {
                if(sLower(e.path().filename().string()) == lower) {
                    next = e.path();
                    break;
                }
            }
        }
        cur = next;
    }
    return cur.string();
}
#endif

std::string GameDir::path(const std::string& path) {
#ifdef __unix__
    const auto exact = sPath + path;
    std::error_code ec;
    if(std::filesystem::exists(exact, ec)) return exact;
    return sResolveCase(sPath, path);
#else
    return sPath + path;
#endif
}

std::string GameDir::settingsPath() {
    return exeDir() + "../settings.txt";
}

std::string GameDir::numbersPath() {
    return exeDir() + "../numbers.txt";
}

std::string GameDir::iBinaryPath() {
    return exeDir() + "../interface.e";
}

std::string GameDir::i15BinaryPath() {
    return exeDir() + "../i15.e";
}

std::string GameDir::i30BinaryPath() {
    return exeDir() + "../i30.e";
}

std::string GameDir::i45BinaryPath() {
    return exeDir() + "../i45.e";
}

std::string GameDir::i60BinaryPath() {
    return exeDir() + "../i60.e";
}

std::string GameDir::exeDir() {
    const auto d = SDL_GetBasePath();
    const std::string str(d);
    return str;
}

std::string GameDir::adventuresDir() {
    return exeDir() + "../Adventures/";
}

std::string GameDir::pakAdventuresDir() {
    // return "/home/ailuropoda/.eZeus/Zeus/Adventures/"; // !!!
    return GameDir::path("Adventures/");
}

std::string GameDir::saveDir() {
    return exeDir() + "../Save/";
}

std::string GameDir::stampsDir() {
    return exeDir() + "../stamps/";
}

std::string GameDir::texturesDir() {
    return exeDir() + "../Textures/";
}

std::string GameDir::cursorsDir() {
    return exeDir() + "../cursors/";
}
