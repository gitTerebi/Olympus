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
    case eHotkeyId::repairTool: return fHotkeyRepairTool;
    case eHotkeyId::undo: return fHotkeyUndo;
    case eHotkeyId::showRoadsOverlay: return fHotkeyShowRoadsOverlay;
    case eHotkeyId::buildRoad: return fHotkeyBuildRoad;
    case eHotkeyId::buildRoadblock: return fHotkeyBuildRoadblock;
    case eHotkeyId::buildMaintenanceOffice: return fHotkeyBuildMaintenanceOffice;
    case eHotkeyId::buildCommonHousing: return fHotkeyBuildCommonHousing;
    case eHotkeyId::buildWatchpost: return fHotkeyBuildWatchpost;
    case eHotkeyId::buildStamp: return fHotkeyBuildStamp;
    case eHotkeyId::stampManager: return fHotkeyStampManager;
    case eHotkeyId::scrollLeft: return fHotkeyScrollLeft;
    case eHotkeyId::scrollRight: return fHotkeyScrollRight;
    case eHotkeyId::scrollUp: return fHotkeyScrollUp;
    case eHotkeyId::scrollDown: return fHotkeyScrollDown;
    case eHotkeyId::bookmark1: return fHotkeyBookmark1;
    case eHotkeyId::bookmark2: return fHotkeyBookmark2;
    case eHotkeyId::bookmark3: return fHotkeyBookmark3;
    case eHotkeyId::bookmark4: return fHotkeyBookmark4;
    case eHotkeyId::menuTab1: return fHotkeyMenuTab1;
    case eHotkeyId::menuTab2: return fHotkeyMenuTab2;
    case eHotkeyId::menuTab3: return fHotkeyMenuTab3;
    case eHotkeyId::menuTab4: return fHotkeyMenuTab4;
    case eHotkeyId::menuTab5: return fHotkeyMenuTab5;
    case eHotkeyId::menuTab6: return fHotkeyMenuTab6;
    case eHotkeyId::menuTab7: return fHotkeyMenuTab7;
    case eHotkeyId::menuTab8: return fHotkeyMenuTab8;
    case eHotkeyId::menuTab9: return fHotkeyMenuTab9;
    case eHotkeyId::menuTab10: return fHotkeyMenuTab10;
    case eHotkeyId::menuTab11: return fHotkeyMenuTab11;
    }
    return SDL_SCANCODE_UNKNOWN;
}

eHotkeyId eSettings::hotkeyIdForScancode(const SDL_Scancode scancode) const {
    if(scancode == SDL_SCANCODE_UNKNOWN) return eHotkeyId::gameMenu; // invalid
    for(int i = static_cast<int>(eHotkeyId::gameMenu);
        i <= static_cast<int>(eHotkeyId::menuTab11); i++) {
        const auto id = static_cast<eHotkeyId>(i);
        if(hotkey(id) == scancode) return id;
    }
    return eHotkeyId::gameMenu; // not found, return invalid
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
    case eHotkeyId::repairTool: fHotkeyRepairTool = key; break;
    case eHotkeyId::undo: fHotkeyUndo = key; break;
    case eHotkeyId::showRoadsOverlay: fHotkeyShowRoadsOverlay = key; break;
    case eHotkeyId::buildRoad: fHotkeyBuildRoad = key; break;
    case eHotkeyId::buildRoadblock: fHotkeyBuildRoadblock = key; break;
    case eHotkeyId::buildMaintenanceOffice: fHotkeyBuildMaintenanceOffice = key; break;
    case eHotkeyId::buildCommonHousing: fHotkeyBuildCommonHousing = key; break;
    case eHotkeyId::buildWatchpost: fHotkeyBuildWatchpost = key; break;
    case eHotkeyId::buildStamp: fHotkeyBuildStamp = key; break;
    case eHotkeyId::stampManager: fHotkeyStampManager = key; break;
    case eHotkeyId::scrollLeft: fHotkeyScrollLeft = key; break;
    case eHotkeyId::scrollRight: fHotkeyScrollRight = key; break;
    case eHotkeyId::scrollUp: fHotkeyScrollUp = key; break;
    case eHotkeyId::scrollDown: fHotkeyScrollDown = key; break;
    case eHotkeyId::bookmark1: fHotkeyBookmark1 = key; break;
    case eHotkeyId::bookmark2: fHotkeyBookmark2 = key; break;
    case eHotkeyId::bookmark3: fHotkeyBookmark3 = key; break;
    case eHotkeyId::bookmark4: fHotkeyBookmark4 = key; break;
    case eHotkeyId::menuTab1: fHotkeyMenuTab1 = key; break;
    case eHotkeyId::menuTab2: fHotkeyMenuTab2 = key; break;
    case eHotkeyId::menuTab3: fHotkeyMenuTab3 = key; break;
    case eHotkeyId::menuTab4: fHotkeyMenuTab4 = key; break;
    case eHotkeyId::menuTab5: fHotkeyMenuTab5 = key; break;
    case eHotkeyId::menuTab6: fHotkeyMenuTab6 = key; break;
    case eHotkeyId::menuTab7: fHotkeyMenuTab7 = key; break;
    case eHotkeyId::menuTab8: fHotkeyMenuTab8 = key; break;
    case eHotkeyId::menuTab9: fHotkeyMenuTab9 = key; break;
    case eHotkeyId::menuTab10: fHotkeyMenuTab10 = key; break;
    case eHotkeyId::menuTab11: fHotkeyMenuTab11 = key; break;
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
}

