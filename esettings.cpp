#include "esettings.h"

#include <fstream>
#include <iostream>
#include <algorithm>

#include "egamedir.h"
#include "eloadtexthelper.h"

std::vector<eTileSize> eSettings::availableSizes() const {
    std::vector<eTileSize> sizes;
    if(fTinyTextures) {
        sizes.push_back(eTileSize::s15);
    }
    if(fSmallTextures) {
        sizes.push_back(eTileSize::s30);
    }
    if(fMediumTextures) {
        sizes.push_back(eTileSize::s45);
    }
    if(fLargeTextures) {
        sizes.push_back(eTileSize::s60);
    }
    return sizes;
}

int eSettings::clampKeyScrollSpeed(const int speed) {
    return std::clamp(speed, sMinKeyScrollSpeed, sMaxKeyScrollSpeed);
}

int eSettings::clampVolume(const int volume) {
    return std::clamp(volume, sMinVolume, sMaxVolume);
}

int eSettings::clampGameSpeed(const int speed) {
    const int clamped = std::clamp(speed, sMinGameSpeed, sMaxGameSpeed);
    const int mid = (sMinGameSpeed + sMaxGameSpeed)/2;
    return clamped < mid ? sMinGameSpeed : sMaxGameSpeed;
}

SDL_Scancode eSettings::hotkey(const eHotkeyId id) const {
    switch(id) {
    case eHotkeyId::gameMenu: return fHotkeyGameMenu;
    case eHotkeyId::speedUp: return fHotkeySpeedUp;
    case eHotkeyId::speedDown: return fHotkeySpeedDown;
    case eHotkeyId::pause: return fHotkeyPause;
    case eHotkeyId::rotatePreview: return fHotkeyRotatePreview;
    case eHotkeyId::copyBuilding: return fHotkeyCopyBuilding;
    case eHotkeyId::deleteTool: return fHotkeyDeleteTool;
    case eHotkeyId::buildRoad: return fHotkeyBuildRoad;
    case eHotkeyId::buildRoadblock: return fHotkeyBuildRoadblock;
    case eHotkeyId::buildMaintenanceOffice: return fHotkeyBuildMaintenanceOffice;
    case eHotkeyId::scrollLeft: return fHotkeyScrollLeft;
    case eHotkeyId::scrollRight: return fHotkeyScrollRight;
    case eHotkeyId::scrollUp: return fHotkeyScrollUp;
    case eHotkeyId::scrollDown: return fHotkeyScrollDown;
    case eHotkeyId::bookmark1: return fHotkeyBookmark1;
    case eHotkeyId::bookmark2: return fHotkeyBookmark2;
    case eHotkeyId::bookmark3: return fHotkeyBookmark3;
    case eHotkeyId::bookmark4: return fHotkeyBookmark4;
    }
    return SDL_SCANCODE_UNKNOWN;
}

void eSettings::setHotkey(const eHotkeyId id, const SDL_Scancode key) {
    switch(id) {
    case eHotkeyId::gameMenu: fHotkeyGameMenu = key; break;
    case eHotkeyId::speedUp: fHotkeySpeedUp = key; break;
    case eHotkeyId::speedDown: fHotkeySpeedDown = key; break;
    case eHotkeyId::pause: fHotkeyPause = key; break;
    case eHotkeyId::rotatePreview: fHotkeyRotatePreview = key; break;
    case eHotkeyId::copyBuilding: fHotkeyCopyBuilding = key; break;
    case eHotkeyId::deleteTool: fHotkeyDeleteTool = key; break;
    case eHotkeyId::buildRoad: fHotkeyBuildRoad = key; break;
    case eHotkeyId::buildRoadblock: fHotkeyBuildRoadblock = key; break;
    case eHotkeyId::buildMaintenanceOffice: fHotkeyBuildMaintenanceOffice = key; break;
    case eHotkeyId::scrollLeft: fHotkeyScrollLeft = key; break;
    case eHotkeyId::scrollRight: fHotkeyScrollRight = key; break;
    case eHotkeyId::scrollUp: fHotkeyScrollUp = key; break;
    case eHotkeyId::scrollDown: fHotkeyScrollDown = key; break;
    case eHotkeyId::bookmark1: fHotkeyBookmark1 = key; break;
    case eHotkeyId::bookmark2: fHotkeyBookmark2 = key; break;
    case eHotkeyId::bookmark3: fHotkeyBookmark3 = key; break;
    case eHotkeyId::bookmark4: fHotkeyBookmark4 = key; break;
    }
}

void eSettings::write() const {
    const auto path = eGameDir::settingsPath();
    std::ofstream file;
    file.open(path);
    file << "tiny_textures" << " " <<
            (fTinyTextures ? "\"true\"" : "\"false\"") << "\n";
    file << "small_textures" << " " <<
            (fSmallTextures ? "\"true\"" : "\"false\"") << "\n";
    file << "medium_textures" << " " <<
            (fMediumTextures ? "\"true\"" : "\"false\"") << "\n";
    file << "large_textures" << " " <<
            (fLargeTextures ? "\"true\"" : "\"false\"") << "\n";
    file << "fullscreen" << " " <<
            (fFullscreen ? "\"true\"" : "\"false\"") << "\n";
    file << "key_scroll_speed" << " " << "\"" <<
            std::to_string(fKeyScrollSpeed) << "\"" << "\n";
    file << "game_speed" << " " << "\"" <<
            std::to_string(fGameSpeed) << "\"" << "\n";
    file << "music_volume" << " " << "\"" <<
            std::to_string(fMusicVolume) << "\"" << "\n";
    file << "sounds_volume" << " " << "\"" <<
            std::to_string(fSoundsVolume) << "\"" << "\n";
    file << "hotkey_game_menu" << " " << "\"" <<
            std::to_string(fHotkeyGameMenu) << "\"" << "\n";
    file << "hotkey_speed_up" << " " << "\"" <<
            std::to_string(fHotkeySpeedUp) << "\"" << "\n";
    file << "hotkey_speed_down" << " " << "\"" <<
            std::to_string(fHotkeySpeedDown) << "\"" << "\n";
    file << "hotkey_pause" << " " << "\"" <<
            std::to_string(fHotkeyPause) << "\"" << "\n";
    file << "hotkey_rotate_preview" << " " << "\"" <<
            std::to_string(fHotkeyRotatePreview) << "\"" << "\n";
    file << "hotkey_copy_building" << " " << "\"" <<
            std::to_string(fHotkeyCopyBuilding) << "\"" << "\n";
    file << "hotkey_delete_tool" << " " << "\"" <<
            std::to_string(fHotkeyDeleteTool) << "\"" << "\n";
    file << "hotkey_build_road" << " " << "\"" <<
            std::to_string(fHotkeyBuildRoad) << "\"" << "\n";
    file << "hotkey_build_roadblock" << " " << "\"" <<
            std::to_string(fHotkeyBuildRoadblock) << "\"" << "\n";
    file << "hotkey_build_maintenance_office" << " " << "\"" <<
            std::to_string(fHotkeyBuildMaintenanceOffice) << "\"" << "\n";
    file << "hotkey_scroll_left" << " " << "\"" <<
            std::to_string(fHotkeyScrollLeft) << "\"" << "\n";
    file << "hotkey_scroll_right" << " " << "\"" <<
            std::to_string(fHotkeyScrollRight) << "\"" << "\n";
    file << "hotkey_scroll_up" << " " << "\"" <<
            std::to_string(fHotkeyScrollUp) << "\"" << "\n";
    file << "hotkey_scroll_down" << " " << "\"" <<
            std::to_string(fHotkeyScrollDown) << "\"" << "\n";
    file << "hotkey_bookmark_1" << " " << "\"" <<
            std::to_string(fHotkeyBookmark1) << "\"" << "\n";
    file << "hotkey_bookmark_2" << " " << "\"" <<
            std::to_string(fHotkeyBookmark2) << "\"" << "\n";
    file << "hotkey_bookmark_3" << " " << "\"" <<
            std::to_string(fHotkeyBookmark3) << "\"" << "\n";
    file << "hotkey_bookmark_4" << " " << "\"" <<
            std::to_string(fHotkeyBookmark4) << "\"" << "\n";
    const auto wStr = std::to_string(fRes.width());
    file << "width" << " " << "\"" << wStr << "\"" << "\n";
    const auto hStr = std::to_string(fRes.height());
    file << "height" << " " << "\"" << hStr << "\"" << "\n";
    file.close();
}

void eSettings::read() {
    const auto path = eGameDir::settingsPath();
    std::map<std::string, std::string> settings;
    const bool r = eLoadTextHelper::load(path, settings);
    if(!r) return;
    fTinyTextures = settings["tiny_textures"] == "true";
    fSmallTextures = settings["small_textures"] == "true";
    fMediumTextures = settings["medium_textures"] == "true";
    fLargeTextures = settings["large_textures"] == "true";
    fFullscreen = settings["fullscreen"] == "true";
    const auto keyScrollSpeedStr = settings["key_scroll_speed"];
    if(!keyScrollSpeedStr.empty()) {
        fKeyScrollSpeed = clampKeyScrollSpeed(std::stoi(keyScrollSpeedStr));
    }
    const auto gameSpeedStr = settings["game_speed"];
    if(!gameSpeedStr.empty()) {
        fGameSpeed = clampGameSpeed(std::stoi(gameSpeedStr));
    }
    const auto musicVolumeStr = settings["music_volume"];
    if(!musicVolumeStr.empty()) {
        fMusicVolume = clampVolume(std::stoi(musicVolumeStr));
    }
    const auto soundsVolumeStr = settings["sounds_volume"];
    if(!soundsVolumeStr.empty()) {
        fSoundsVolume = clampVolume(std::stoi(soundsVolumeStr));
    }
    const auto readHotkey = [&settings](const std::string& key,
                                        SDL_Scancode& value) {
        const auto str = settings[key];
        if(str.empty()) return;
        const int code = std::stoi(str);
        if(code >= SDL_SCANCODE_UNKNOWN && code < SDL_NUM_SCANCODES) {
            value = static_cast<SDL_Scancode>(code);
        }
    };
    readHotkey("hotkey_game_menu", fHotkeyGameMenu);
    readHotkey("hotkey_speed_up", fHotkeySpeedUp);
    readHotkey("hotkey_speed_down", fHotkeySpeedDown);
    readHotkey("hotkey_pause", fHotkeyPause);
    readHotkey("hotkey_rotate_preview", fHotkeyRotatePreview);
    readHotkey("hotkey_copy_building", fHotkeyCopyBuilding);
    readHotkey("hotkey_delete_tool", fHotkeyDeleteTool);
    readHotkey("hotkey_build_road", fHotkeyBuildRoad);
    readHotkey("hotkey_build_roadblock", fHotkeyBuildRoadblock);
    readHotkey("hotkey_build_maintenance_office", fHotkeyBuildMaintenanceOffice);
    readHotkey("hotkey_scroll_left", fHotkeyScrollLeft);
    readHotkey("hotkey_scroll_right", fHotkeyScrollRight);
    readHotkey("hotkey_scroll_up", fHotkeyScrollUp);
    readHotkey("hotkey_scroll_down", fHotkeyScrollDown);
    readHotkey("hotkey_bookmark_1", fHotkeyBookmark1);
    readHotkey("hotkey_bookmark_2", fHotkeyBookmark2);
    readHotkey("hotkey_bookmark_3", fHotkeyBookmark3);
    readHotkey("hotkey_bookmark_4", fHotkeyBookmark4);
    if(fHotkeyRotatePreview == SDL_SCANCODE_R &&
       settings["hotkey_build_road"].empty()) {
        fHotkeyRotatePreview = SDL_SCANCODE_TAB;
    }
    const auto widthStr = settings["width"];
    const auto heightStr = settings["height"];
    if(!widthStr.empty() && !heightStr.empty()) {
        const int width = std::stoi(widthStr);
        const int height = std::stoi(heightStr);
        fRes = eResolution(width, height);
    }
}

