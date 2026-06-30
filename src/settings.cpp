#include "settings.h"

#include <fstream>
#include <iostream>
#include <algorithm>

#include "game-dir.h"
#include "load-text-helper.h"

std::vector<eTileSize> Settings::availableSizes() const {
    // Only the s30 tile texture set ships. The tiny/medium/large flags don't
    // correspond to real assets, and honoring them made the tile draw size
    // change (and seam) depending on which flags happened to be enabled.
    return { eTileSize::s30 };
}

int Settings::clampKeyScrollSpeed(const int speed) {
    return std::clamp(speed, sMinKeyScrollSpeed, sMaxKeyScrollSpeed);
}

int Settings::clampVolume(const int volume) {
    return std::clamp(volume, sMinVolume, sMaxVolume);
}

int Settings::clampGameSpeed(const int speed) {
    const int clamped = std::clamp(speed, sMinGameSpeed, sMaxGameSpeed);
    const int mid = (sMinGameSpeed + sMaxGameSpeed)/2;
    return clamped < mid ? sMinGameSpeed : sMaxGameSpeed;
}

int Settings::clampTopSidebarScale(const int scale) {
    return std::clamp(scale, 0, 3);
}

SDL_Scancode Settings::hotkey(const HotkeyId id) const {
    switch(id) {
    case HotkeyId::gameMenu: return fHotkeyGameMenu;
    case HotkeyId::speedUp: return fHotkeySpeedUp;
    case HotkeyId::speedDown: return fHotkeySpeedDown;
    case HotkeyId::pause: return fHotkeyPause;
    case HotkeyId::rotatePreview: return fHotkeyRotatePreview;
    case HotkeyId::copyBuilding: return fHotkeyCopyBuilding;
    case HotkeyId::deleteTool: return fHotkeyDeleteTool;
    case HotkeyId::repairTool: return fHotkeyRepairTool;
    case HotkeyId::undo: return fHotkeyUndo;
    case HotkeyId::showRoadsOverlay: return fHotkeyShowRoadsOverlay;
    case HotkeyId::buildRoad: return fHotkeyBuildRoad;
    case HotkeyId::buildRoadblock: return fHotkeyBuildRoadblock;
    case HotkeyId::buildMaintenanceOffice: return fHotkeyBuildMaintenanceOffice;
    case HotkeyId::buildCommonHousing: return fHotkeyBuildCommonHousing;
    case HotkeyId::buildWatchpost: return fHotkeyBuildWatchpost;
    case HotkeyId::buildStamp: return fHotkeyBuildStamp;
    case HotkeyId::stampManager: return fHotkeyStampManager;
    case HotkeyId::scrollLeft: return fHotkeyScrollLeft;
    case HotkeyId::scrollRight: return fHotkeyScrollRight;
    case HotkeyId::scrollUp: return fHotkeyScrollUp;
    case HotkeyId::scrollDown: return fHotkeyScrollDown;
    case HotkeyId::bookmark1: return fHotkeyBookmark1;
    case HotkeyId::bookmark2: return fHotkeyBookmark2;
    case HotkeyId::bookmark3: return fHotkeyBookmark3;
    case HotkeyId::bookmark4: return fHotkeyBookmark4;
    case HotkeyId::menuTab1: return fHotkeyMenuTab1;
    case HotkeyId::menuTab2: return fHotkeyMenuTab2;
    case HotkeyId::menuTab3: return fHotkeyMenuTab3;
    case HotkeyId::menuTab4: return fHotkeyMenuTab4;
    case HotkeyId::menuTab5: return fHotkeyMenuTab5;
    case HotkeyId::menuTab6: return fHotkeyMenuTab6;
    case HotkeyId::menuTab7: return fHotkeyMenuTab7;
    case HotkeyId::menuTab8: return fHotkeyMenuTab8;
    case HotkeyId::menuTab9: return fHotkeyMenuTab9;
    case HotkeyId::menuTab10: return fHotkeyMenuTab10;
    case HotkeyId::menuTab11: return fHotkeyMenuTab11;
    }
    return SDL_SCANCODE_UNKNOWN;
}

HotkeyId Settings::hotkeyIdForScancode(const SDL_Scancode scancode) const {
    if(scancode == SDL_SCANCODE_UNKNOWN) return HotkeyId::gameMenu; // invalid
    for(int i = static_cast<int>(HotkeyId::gameMenu);
        i <= static_cast<int>(HotkeyId::menuTab11); i++) {
        const auto id = static_cast<HotkeyId>(i);
        if(hotkey(id) == scancode) return id;
    }
    return HotkeyId::gameMenu; // not found, return invalid
}

void Settings::setHotkey(const HotkeyId id, const SDL_Scancode key) {
    switch(id) {
    case HotkeyId::gameMenu: fHotkeyGameMenu = key; break;
    case HotkeyId::speedUp: fHotkeySpeedUp = key; break;
    case HotkeyId::speedDown: fHotkeySpeedDown = key; break;
    case HotkeyId::pause: fHotkeyPause = key; break;
    case HotkeyId::rotatePreview: fHotkeyRotatePreview = key; break;
    case HotkeyId::copyBuilding: fHotkeyCopyBuilding = key; break;
    case HotkeyId::deleteTool: fHotkeyDeleteTool = key; break;
    case HotkeyId::repairTool: fHotkeyRepairTool = key; break;
    case HotkeyId::undo: fHotkeyUndo = key; break;
    case HotkeyId::showRoadsOverlay: fHotkeyShowRoadsOverlay = key; break;
    case HotkeyId::buildRoad: fHotkeyBuildRoad = key; break;
    case HotkeyId::buildRoadblock: fHotkeyBuildRoadblock = key; break;
    case HotkeyId::buildMaintenanceOffice: fHotkeyBuildMaintenanceOffice = key; break;
    case HotkeyId::buildCommonHousing: fHotkeyBuildCommonHousing = key; break;
    case HotkeyId::buildWatchpost: fHotkeyBuildWatchpost = key; break;
    case HotkeyId::buildStamp: fHotkeyBuildStamp = key; break;
    case HotkeyId::stampManager: fHotkeyStampManager = key; break;
    case HotkeyId::scrollLeft: fHotkeyScrollLeft = key; break;
    case HotkeyId::scrollRight: fHotkeyScrollRight = key; break;
    case HotkeyId::scrollUp: fHotkeyScrollUp = key; break;
    case HotkeyId::scrollDown: fHotkeyScrollDown = key; break;
    case HotkeyId::bookmark1: fHotkeyBookmark1 = key; break;
    case HotkeyId::bookmark2: fHotkeyBookmark2 = key; break;
    case HotkeyId::bookmark3: fHotkeyBookmark3 = key; break;
    case HotkeyId::bookmark4: fHotkeyBookmark4 = key; break;
    case HotkeyId::menuTab1: fHotkeyMenuTab1 = key; break;
    case HotkeyId::menuTab2: fHotkeyMenuTab2 = key; break;
    case HotkeyId::menuTab3: fHotkeyMenuTab3 = key; break;
    case HotkeyId::menuTab4: fHotkeyMenuTab4 = key; break;
    case HotkeyId::menuTab5: fHotkeyMenuTab5 = key; break;
    case HotkeyId::menuTab6: fHotkeyMenuTab6 = key; break;
    case HotkeyId::menuTab7: fHotkeyMenuTab7 = key; break;
    case HotkeyId::menuTab8: fHotkeyMenuTab8 = key; break;
    case HotkeyId::menuTab9: fHotkeyMenuTab9 = key; break;
    case HotkeyId::menuTab10: fHotkeyMenuTab10 = key; break;
    case HotkeyId::menuTab11: fHotkeyMenuTab11 = key; break;
    }
}

void Settings::write() const {
    const auto path = GameDir::settingsPath();
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
    file << "display_mode" << " " << "\"" <<
            std::to_string(static_cast<int>(fDisplayMode)) << "\"" << "\n";
    file << "warehouse_default_accept_none" << " " <<
            (fWarehouseDefaultAcceptNone ? "\"true\"" : "\"false\"") << "\n";
    file << "double_cart_capacity" << " " <<
            (fDoubleCartCapacity ? "\"true\"" : "\"false\"") << "\n";
    file << "agoras_take_from_trading_posts" << " " <<
            (fAgorasTakeFromTradingPosts ? "\"true\"" : "\"false\"") << "\n";
    file << "enable_yearly_autosaves" << " " <<
            (fEnableYearlyAutosaves ? "\"true\"" : "\"false\"") << "\n";
    file << "popup_for_invasion" << " " <<
            (fPopupForInvasion ? "\"true\"" : "\"false\"") << "\n";
    file << "popup_for_requests" << " " <<
            (fPopupForRequests ? "\"true\"" : "\"false\"") << "\n";
    file << "popup_for_tributes" << " " <<
            (fPopupForTributes ? "\"true\"" : "\"false\"") << "\n";
    file << "popup_for_troops" << " " <<
            (fPopupForTroops ? "\"true\"" : "\"false\"") << "\n";
    file << "disable_edge_scroll" << " " <<
            (fDisableEdgeScroll ? "\"true\"" : "\"false\"") << "\n";
    file << "last_difficulty" << " " << "\"" <<
            std::to_string(static_cast<int>(fLastDifficulty)) << "\"" << "\n";
    file << "key_scroll_speed" << " " << "\"" <<
            std::to_string(fKeyScrollSpeed) << "\"" << "\n";
    file << "game_speed" << " " << "\"" <<
            std::to_string(fGameSpeed) << "\"" << "\n";
    file << "ui_scale" << " " << "\"" <<
            std::to_string(static_cast<int>(fUiScale)) << "\"" << "\n";
    file << "top_sidebar_scale" << " " << "\"" <<
            std::to_string(fTopSidebarScale) << "\"" << "\n";
    file << "interpolation" << " " << "\"" <<
            std::to_string(static_cast<int>(fInterpolation)) << "\"" << "\n";
    file << "upscale" << " " << "\"" <<
            std::to_string(static_cast<int>(fUpscale)) << "\"" << "\n";
    file << "upscale_factor" << " " << "\"" <<
            std::to_string(fUpscaleFactor) << "\"" << "\n";
    file << "general_volume" << " " << "\"" <<
            std::to_string(fGeneralVolume) << "\"" << "\n";
    file << "music_volume" << " " << "\"" <<
            std::to_string(fMusicVolume) << "\"" << "\n";
    file << "voice_volume" << " " << "\"" <<
            std::to_string(fVoiceVolume) << "\"" << "\n";
    file << "event_volume" << " " << "\"" <<
            std::to_string(fEventVolume) << "\"" << "\n";
    file << "ambient_volume" << " " << "\"" <<
            std::to_string(fAmbientVolume) << "\"" << "\n";
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
    file << "hotkey_repair_tool" << " " << "\"" <<
            std::to_string(fHotkeyRepairTool) << "\"" << "\n";
    file << "hotkey_undo" << " " << "\"" <<
            std::to_string(fHotkeyUndo) << "\"" << "\n";
    file << "hotkey_show_roads_overlay" << " " << "\"" <<
            std::to_string(fHotkeyShowRoadsOverlay) << "\"" << "\n";
    file << "hotkey_build_road" << " " << "\"" <<
            std::to_string(fHotkeyBuildRoad) << "\"" << "\n";
    file << "hotkey_build_roadblock" << " " << "\"" <<
            std::to_string(fHotkeyBuildRoadblock) << "\"" << "\n";
    file << "hotkey_build_maintenance_office" << " " << "\"" <<
            std::to_string(fHotkeyBuildMaintenanceOffice) << "\"" << "\n";
    file << "hotkey_build_common_housing" << " " << "\"" <<
            std::to_string(fHotkeyBuildCommonHousing) << "\"" << "\n";
    file << "hotkey_build_watchpost" << " " << "\"" <<
            std::to_string(fHotkeyBuildWatchpost) << "\"" << "\n";
    file << "hotkey_build_stamp" << " " << "\"" <<
            std::to_string(fHotkeyBuildStamp) << "\"" << "\n";
    file << "hotkey_stamp_manager" << " " << "\"" <<
            std::to_string(fHotkeyStampManager) << "\"" << "\n";
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
    file << "hotkey_menu_tab_1" << " " << "\"" <<
            std::to_string(fHotkeyMenuTab1) << "\"" << "\n";
    file << "hotkey_menu_tab_2" << " " << "\"" <<
            std::to_string(fHotkeyMenuTab2) << "\"" << "\n";
    file << "hotkey_menu_tab_3" << " " << "\"" <<
            std::to_string(fHotkeyMenuTab3) << "\"" << "\n";
    file << "hotkey_menu_tab_4" << " " << "\"" <<
            std::to_string(fHotkeyMenuTab4) << "\"" << "\n";
    file << "hotkey_menu_tab_5" << " " << "\"" <<
            std::to_string(fHotkeyMenuTab5) << "\"" << "\n";
    file << "hotkey_menu_tab_6" << " " << "\"" <<
            std::to_string(fHotkeyMenuTab6) << "\"" << "\n";
    file << "hotkey_menu_tab_7" << " " << "\"" <<
            std::to_string(fHotkeyMenuTab7) << "\"" << "\n";
    file << "hotkey_menu_tab_8" << " " << "\"" <<
            std::to_string(fHotkeyMenuTab8) << "\"" << "\n";
    file << "hotkey_menu_tab_9" << " " << "\"" <<
            std::to_string(fHotkeyMenuTab9) << "\"" << "\n";
    file << "hotkey_menu_tab_10" << " " << "\"" <<
            std::to_string(fHotkeyMenuTab10) << "\"" << "\n";
    file << "hotkey_menu_tab_11" << " " << "\"" <<
            std::to_string(fHotkeyMenuTab11) << "\"" << "\n";
    const auto wStr = std::to_string(fRes.width());
    file << "width" << " " << "\"" << wStr << "\"" << "\n";
    const auto hStr = std::to_string(fRes.height());
    file << "height" << " " << "\"" << hStr << "\"" << "\n";
    file.close();
}

void Settings::read() {
    const auto path = GameDir::settingsPath();
    std::map<std::string, std::string> settings;
    const bool r = LoadTextHelper::load(path, settings);
    if(!r) return;
    fTinyTextures = settings["tiny_textures"] == "true";
    fSmallTextures = settings["small_textures"] == "true";
    fMediumTextures = settings["medium_textures"] == "true";
    fLargeTextures = settings["large_textures"] == "true";
    const auto displayModeStr = settings["display_mode"];
    if(!displayModeStr.empty()) {
        const int v = std::stoi(displayModeStr);
        if(v >= 0 && v < static_cast<int>(DisplayMode::count)) {
            fDisplayMode = static_cast<DisplayMode>(v);
        }
    } else if(settings["fullscreen"] == "true") {
        fDisplayMode = DisplayMode::fullscreen;
    }
    fWarehouseDefaultAcceptNone = settings["warehouse_default_accept_none"] == "true";
    fDoubleCartCapacity = settings["double_cart_capacity"] == "true";
    fAgorasTakeFromTradingPosts = settings["agoras_take_from_trading_posts"] == "true";
    const auto enableYearlyAutosavesStr = settings["enable_yearly_autosaves"];
    if(!enableYearlyAutosavesStr.empty()) {
        fEnableYearlyAutosaves = enableYearlyAutosavesStr == "true";
    }
    const auto popupForInvasionStr = settings["popup_for_invasion"];
    if(!popupForInvasionStr.empty()) fPopupForInvasion = popupForInvasionStr == "true";
    const auto popupForRequestsStr = settings["popup_for_requests"];
    if(!popupForRequestsStr.empty()) fPopupForRequests = popupForRequestsStr == "true";
    const auto popupForTributesStr = settings["popup_for_tributes"];
    if(!popupForTributesStr.empty()) fPopupForTributes = popupForTributesStr == "true";
    const auto popupForTroopsStr = settings["popup_for_troops"];
    if(!popupForTroopsStr.empty()) fPopupForTroops = popupForTroopsStr == "true";
    fDisableEdgeScroll = settings["disable_edge_scroll"] == "true";
    const auto lastDiffStr = settings["last_difficulty"];
    if(!lastDiffStr.empty()) {
        fLastDifficulty = static_cast<Difficulty>(std::stoi(lastDiffStr));
    }
    const auto keyScrollSpeedStr = settings["key_scroll_speed"];
    if(!keyScrollSpeedStr.empty()) {
        fKeyScrollSpeed = clampKeyScrollSpeed(std::stoi(keyScrollSpeedStr));
    }
    const auto gameSpeedStr = settings["game_speed"];
    if(!gameSpeedStr.empty()) {
        fGameSpeed = clampGameSpeed(std::stoi(gameSpeedStr));
    }
    const auto uiScaleStr = settings["ui_scale"];
    if(!uiScaleStr.empty()) {
        const int v = std::stoi(uiScaleStr);
        if(v >= static_cast<int>(eUIScale::tiny) &&
           v <= static_cast<int>(eUIScale::large)) {
            fUiScale = static_cast<eUIScale>(v);
        }
    }
    const auto topSidebarScaleStr = settings["top_sidebar_scale"];
    if(!topSidebarScaleStr.empty()) {
        fTopSidebarScale = clampTopSidebarScale(std::stoi(topSidebarScaleStr));
    }
    const auto interpolationStr = settings["interpolation"];
    if(!interpolationStr.empty()) {
        const int v = std::stoi(interpolationStr);
        if(v >= 0 && v < static_cast<int>(Interpolation::count)) {
            fInterpolation = static_cast<Interpolation>(v);
        }
    }
    const auto upscaleStr = settings["upscale"];
    if(!upscaleStr.empty()) {
        const int v = std::stoi(upscaleStr);
        if(v >= 0 && v < static_cast<int>(Upscale::count)) {
            fUpscale = static_cast<Upscale>(v);
        }
    }
    const auto upscaleFactorStr = settings["upscale_factor"];
    if(!upscaleFactorStr.empty()) {
        const int v = std::stoi(upscaleFactorStr);
        fUpscaleFactor = (v < 2) ? 2 : (v > 6 ? 6 : v);
    }
    const auto generalVolumeStr = settings["general_volume"];
    if(!generalVolumeStr.empty()) {
        fGeneralVolume = clampVolume(std::stoi(generalVolumeStr));
    }
    const auto musicVolumeStr = settings["music_volume"];
    if(!musicVolumeStr.empty()) {
        fMusicVolume = clampVolume(std::stoi(musicVolumeStr));
    }
    const auto soundsVolumeStr = settings["sounds_volume"];
    if(!soundsVolumeStr.empty()) {
        const int soundsVolume = clampVolume(std::stoi(soundsVolumeStr));
        fVoiceVolume = soundsVolume;
        fEventVolume = soundsVolume;
        fAmbientVolume = soundsVolume;
    }
    const auto voiceVolumeStr = settings["voice_volume"];
    if(!voiceVolumeStr.empty()) {
        fVoiceVolume = clampVolume(std::stoi(voiceVolumeStr));
    }
    const auto eventVolumeStr = settings["event_volume"];
    if(!eventVolumeStr.empty()) {
        fEventVolume = clampVolume(std::stoi(eventVolumeStr));
    }
    const auto ambientVolumeStr = settings["ambient_volume"];
    if(!ambientVolumeStr.empty()) {
        fAmbientVolume = clampVolume(std::stoi(ambientVolumeStr));
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
    readHotkey("hotkey_undo", fHotkeyUndo);
    readHotkey("hotkey_show_roads_overlay", fHotkeyShowRoadsOverlay);
    readHotkey("hotkey_build_road", fHotkeyBuildRoad);
    readHotkey("hotkey_build_roadblock", fHotkeyBuildRoadblock);
    readHotkey("hotkey_build_maintenance_office", fHotkeyBuildMaintenanceOffice);
    readHotkey("hotkey_build_common_housing", fHotkeyBuildCommonHousing);
    readHotkey("hotkey_build_watchpost", fHotkeyBuildWatchpost);
    readHotkey("hotkey_build_stamp", fHotkeyBuildStamp);
    readHotkey("hotkey_stamp_manager", fHotkeyStampManager);
    readHotkey("hotkey_build_repair", fHotkeyRepairTool);
    readHotkey("hotkey_scroll_left", fHotkeyScrollLeft);
    readHotkey("hotkey_scroll_right", fHotkeyScrollRight);
    readHotkey("hotkey_scroll_up", fHotkeyScrollUp);
    readHotkey("hotkey_scroll_down", fHotkeyScrollDown);
    readHotkey("hotkey_bookmark_1", fHotkeyBookmark1);
    readHotkey("hotkey_bookmark_2", fHotkeyBookmark2);
    readHotkey("hotkey_bookmark_3", fHotkeyBookmark3);
    readHotkey("hotkey_bookmark_4", fHotkeyBookmark4);
    readHotkey("hotkey_menu_tab_1", fHotkeyMenuTab1);
    readHotkey("hotkey_menu_tab_2", fHotkeyMenuTab2);
    readHotkey("hotkey_menu_tab_3", fHotkeyMenuTab3);
    readHotkey("hotkey_menu_tab_4", fHotkeyMenuTab4);
    readHotkey("hotkey_menu_tab_5", fHotkeyMenuTab5);
    readHotkey("hotkey_menu_tab_6", fHotkeyMenuTab6);
    readHotkey("hotkey_menu_tab_7", fHotkeyMenuTab7);
    readHotkey("hotkey_menu_tab_8", fHotkeyMenuTab8);
    readHotkey("hotkey_menu_tab_9", fHotkeyMenuTab9);
    readHotkey("hotkey_menu_tab_10", fHotkeyMenuTab10);
    readHotkey("hotkey_menu_tab_11", fHotkeyMenuTab11);
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
    if(uiScaleStr.empty()) {
        fUiScale = fRes.uiScale();
    } else {
        fRes = eResolution(fRes.width(), fRes.height(), fUiScale);
    }
}
