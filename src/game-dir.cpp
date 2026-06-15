#include "game-dir.h"

#include <SDL2/SDL_filesystem.h>
#include <cstdio>
#include <fstream>

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
    sPath = exeDir() + str;
    printf("Game data path: %s (from %s)\n", sPath.c_str(), zp.c_str());
}

std::string GameDir::path(const std::string& path) {
    return sPath + path;
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
