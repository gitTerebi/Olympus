#include "eoptionsdata.h"

#include "main-window.h"
#include "settings.h"
#include "language.h"
#include "engine/game-board.h"
#include "widgets/game-widget.h"

namespace {
std::string aspectName(const eResolution& res) {
    const int w = res.width();
    const int h = res.height();
    if((w == 1138 && h == 600) ||
       (w == 1259 && h == 664) ||
       (w == 1395 && h == 736) ||
       (w == 1532 && h == 808) ||
       (w == 1668 && h == 880) ||
       (w == 1805 && h == 952) ||
       (w == 1942 && h == 1024) ||
       (w == 2078 && h == 1096) ||
       (w == 2214 && h == 1168) ||
       (w == 2493 && h == 1315)) {
        return "Desktop";
    }
    if(w * 3 == h * 4) return "4:3";
    if(w * 9 == h * 16) return "16:9";
    if(w * 10 == h * 16) return "16:10";
    return "Other";
}

std::vector<int> resolutionIndicesForAspect(const std::string& aspect) {
    std::vector<int> result;
    for(int i = 0; i < static_cast<int>(eResolution::sResolutions.size()); i++) {
        if(aspectName(eResolution::sResolutions[i]) == aspect) {
            result.push_back(i);
        }
    }
    return result;
}

int bestResolutionForAspect(const std::string& aspect,
                            const eResolution& current) {
    const auto indices = resolutionIndicesForAspect(aspect);
    if(indices.empty()) return -1;

    int best = indices[0];
    int bestScore = 1000000000;
    for(const int i : indices) {
        const auto& res = eResolution::sResolutions[i];
        int score = std::abs(res.height() - current.height());
        if(res.uiScale() != current.uiScale()) score += 10000;
        if(score < bestScore) {
            best = i;
            bestScore = score;
        }
    }
    return best;
}

std::vector<std::string> aspectOptionsForDisplayMode(const DisplayMode mode) {
    if(mode == DisplayMode::window) {
        return {"Desktop"};
    }
    return {"4:3", "16:9", "16:10"};
}

std::string firstAllowedAspect(const std::vector<std::string>& options,
                               const std::string& currentAspect) {
    for(const auto& option : options) {
        if(option == currentAspect) return currentAspect;
    }
    if(options.empty()) return currentAspect;
    return options.front();
}

int bestResolutionForDisplayMode(const DisplayMode mode,
                                 const eResolution& current) {
    const auto options = aspectOptionsForDisplayMode(mode);
    const auto aspect = firstAllowedAspect(options, aspectName(current));
    return bestResolutionForAspect(aspect, current);
}
}

std::vector<eOptionsMenu::ePage> getOptionsPages(MainWindow* const window,
                                                 GameBoard* const board,
                                                 GameWidget* const gw) {
    const auto& settings = window->settings();
    const auto aspectOptions = aspectOptionsForDisplayMode(settings.fDisplayMode);
    const std::string currentAspect =
        firstAllowedAspect(aspectOptions, aspectName(settings.fRes));
    int aspectValue = 0;
    for(int i = 0; i < static_cast<int>(aspectOptions.size()); i++) {
        if(aspectOptions[i] == currentAspect) {
            aspectValue = i;
            break;
        }
    }

    const auto resolutionIndices = resolutionIndicesForAspect(currentAspect);
    std::vector<std::string> resolutionOptions;
    int resolutionValue = 0;
    for(int i = 0; i < static_cast<int>(resolutionIndices.size()); i++) {
        const auto globalIndex = resolutionIndices[i];
        const auto& res = eResolution::sResolutions[globalIndex];
        resolutionOptions.push_back(res.name());
        if(res == settings.fRes) resolutionValue = i;
    }
    std::vector<eOptionsMenu::eChoiceItem> displayChoices = {
        {"--- UI ---", {}, 0, nullptr},
        {
            "UI scale",
            {"0.75x", "1x", "1.25x", "1.5x", "1.75x", "2x"},
            static_cast<int>(settings.fUiScale),
            [window](const int v) { window->setUiScale(v); },
            nullptr
        },
        {
            "Top/sidebar factor",
            {"1", "2", "3", "4"},
            Settings::clampTopSidebarScale(settings.fTopSidebarScale),
            [window](const int v) { window->setTopSidebarScale(v); },
            nullptr
        },
        {"--- Resolution ---", {}, 0, nullptr}
    };
    if(settings.fDisplayMode != DisplayMode::window) {
        displayChoices.push_back({
            "Aspect",
            aspectOptions,
            aspectValue,
            [window, aspectOptions](const int v) {
                if(v < 0 || v >= static_cast<int>(aspectOptions.size())) return;
                const int resolution = bestResolutionForAspect(
                    aspectOptions[v],
                    window->settings().fRes);
                if(resolution >= 0) window->setResolution(resolution);
            },
            nullptr
        });
    }
    displayChoices.push_back({
        "Resolution",
        resolutionOptions,
        resolutionValue,
        [window, resolutionIndices](const int v) {
            if(v < 0 || v >= static_cast<int>(resolutionIndices.size())) return;
            window->setResolution(resolutionIndices[v]);
        },
        nullptr
    });
    displayChoices.push_back({
        "Display",
        {"Window", "Borderless", "Fullscreen"},
        static_cast<int>(settings.fDisplayMode),
        [window](const int v) {
            const auto mode = static_cast<DisplayMode>(v);
            window->setDisplayMode(mode);
            const int resolution = bestResolutionForDisplayMode(
                mode,
                window->settings().fRes);
            if(resolution >= 0) window->setResolution(resolution);
        }
    });
    displayChoices.push_back({"--- Shader ---", {}, 0, nullptr});
    displayChoices.push_back({
        "Filter",
        {"Nearest", "Linear", "Hermite", "Cubic", "Lanczos"},
        static_cast<int>(settings.fInterpolation),
        [window](const int v) { window->setInterpolation(v); }
    });
    displayChoices.push_back({
        "Upscale",
        {"None", "xBRZ", "ScaleHQ", "ScaleNx", "Eagle", "xSal"},
        static_cast<int>(settings.fUpscale),
        [window](const int v) { window->setUpscale(v); }
    });
    displayChoices.push_back({
        "Upscale factor",
        {"2x", "3x", "4x"},
        settings.fUpscaleFactor >= 4 ? 2 :
            (settings.fUpscaleFactor >= 3 ? 1 : 0),
        [window](const int v) { window->setUpscaleFactor(v == 2 ? 4 : (v == 1 ? 3 : 2)); }
    });
    std::vector<eOptionsMenu::ePage> pages = {
        {
            "General",
            "General Options",
            {
                {
                    "Key scroll speed",
                    Settings::sMinKeyScrollSpeed,
                    Settings::sMaxKeyScrollSpeed,
                    settings.fKeyScrollSpeed,
                    "",
                    [](const int v) {
                        return Settings::clampKeyScrollSpeed(v);
                    },
                    [window](const int speed) {
                        window->setKeyScrollSpeed(speed);
                    }
                }
            },
            {},
            {},
            {
                {
                    "Disable edge scroll",
                    settings.fDisableEdgeScroll,
                    [window](const bool b) {
                        window->setDisableEdgeScroll(b);
                    },
                    "Mouse at screen edge won't auto-scroll the map."
                }
            }
        },
        {
            "Display",
            "Display Options",
            {},
            {},
            {},
            {},
            {}, // fDifficulties
            displayChoices
        },
        {
            "Hotkeys",
            "Hotkeys",
            {},
            {
                {"Game menu", HotkeyId::gameMenu, settings.fHotkeyGameMenu,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Pause game", HotkeyId::pause, settings.fHotkeyPause,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"--- Speed Control ---", HotkeyId::speedUp, settings.fHotkeySpeedUp, nullptr},
                {"Increase game speed", HotkeyId::speedUp, settings.fHotkeySpeedUp,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Decrease game speed", HotkeyId::speedDown, settings.fHotkeySpeedDown,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"--- Building Tools ---", HotkeyId::rotatePreview, settings.fHotkeyRotatePreview, nullptr},
                {"Rotate building preview", HotkeyId::rotatePreview, settings.fHotkeyRotatePreview,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Copy hovered building mode", HotkeyId::copyBuilding, settings.fHotkeyCopyBuilding,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Delete tool", HotkeyId::deleteTool, settings.fHotkeyDeleteTool,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Repair tool", HotkeyId::repairTool, settings.fHotkeyRepairTool,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Undo last action", HotkeyId::undo, settings.fHotkeyUndo,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"--- Construction ---", HotkeyId::buildRoad, settings.fHotkeyBuildRoad, nullptr},
                {"Build road", HotkeyId::buildRoad, settings.fHotkeyBuildRoad,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Roadblock", HotkeyId::buildRoadblock, settings.fHotkeyBuildRoadblock,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Maintenance office", HotkeyId::buildMaintenanceOffice, settings.fHotkeyBuildMaintenanceOffice,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Common housing", HotkeyId::buildCommonHousing, settings.fHotkeyBuildCommonHousing,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Watchpost", HotkeyId::buildWatchpost, settings.fHotkeyBuildWatchpost,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Stamp tool", HotkeyId::buildStamp, settings.fHotkeyBuildStamp,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Display stamp manager", HotkeyId::stampManager, settings.fHotkeyStampManager,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Show roads overlay", HotkeyId::showRoadsOverlay, settings.fHotkeyShowRoadsOverlay,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"--- Camera ---", HotkeyId::scrollLeft, settings.fHotkeyScrollLeft, nullptr},
                {"Smooth scroll left", HotkeyId::scrollLeft, settings.fHotkeyScrollLeft,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Smooth scroll right", HotkeyId::scrollRight, settings.fHotkeyScrollRight,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Smooth scroll up", HotkeyId::scrollUp, settings.fHotkeyScrollUp,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Smooth scroll down", HotkeyId::scrollDown, settings.fHotkeyScrollDown,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"--- Bookmarks ---", HotkeyId::bookmark1, settings.fHotkeyBookmark1, nullptr},
                {"Bookmark 1", HotkeyId::bookmark1, settings.fHotkeyBookmark1,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Bookmark 2", HotkeyId::bookmark2, settings.fHotkeyBookmark2,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Bookmark 3", HotkeyId::bookmark3, settings.fHotkeyBookmark3,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Bookmark 4", HotkeyId::bookmark4, settings.fHotkeyBookmark4,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"--- Menu Tabs ---", HotkeyId::menuTab1, settings.fHotkeyMenuTab1, nullptr},
                {"Population tab", HotkeyId::menuTab1, settings.fHotkeyMenuTab1,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Husbandry tab", HotkeyId::menuTab2, settings.fHotkeyMenuTab2,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Industry tab", HotkeyId::menuTab3, settings.fHotkeyMenuTab3,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Distribution tab", HotkeyId::menuTab4, settings.fHotkeyMenuTab4,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Hygiene & safety tab", HotkeyId::menuTab5, settings.fHotkeyMenuTab5,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Administration tab", HotkeyId::menuTab6, settings.fHotkeyMenuTab6,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Culture tab", HotkeyId::menuTab7, settings.fHotkeyMenuTab7,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Mythology tab", HotkeyId::menuTab8, settings.fHotkeyMenuTab8,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Military tab", HotkeyId::menuTab9, settings.fHotkeyMenuTab9,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Aesthetics tab", HotkeyId::menuTab10, settings.fHotkeyMenuTab10,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Overview tab", HotkeyId::menuTab11, settings.fHotkeyMenuTab11,
                 [window](const HotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }}
            },
            {
                "Numpad +: increase game speed",
                "Numpad -: decrease game speed",
                "Arrow keys: scroll map",
                "Bookmark key: view bookmark",
                "Ctrl+bookmark key: set bookmark"
            }
        },
        {
            "Gameplay",
            "Gameplay Options",
            {},
            {},
            {},
            {
                {
                    "Warehouse Default Accept None",
                    settings.fWarehouseDefaultAcceptNone,
                    [window](const bool b) {
                        window->setWarehouseDefaultAcceptNone(b);
                    },
                    "New warehouses accept no goods by default."
                },
                {
                    "Double cart capacity",
                    settings.fDoubleCartCapacity,
                    [window](const bool b) {
                        window->setDoubleCartCapacity(b);
                    },
                    "Carts carry 8 items instead of 4."
                },
                {
                    "Agoras take from trading posts",
                    settings.fAgorasTakeFromTradingPosts,
                    [window](const bool b) {
                        window->setAgorasTakeFromTradingPosts(b);
                    },
                    "Allow agora vendors to take goods from trading posts. When disabled (default), they only use warehouses and granaries."
                },
                {
                    "Enable yearly autosaves",
                    settings.fEnableYearlyAutosaves,
                    [window](const bool b) {
                        window->setEnableYearlyAutosaves(b);
                    },
                    "Save automatically at the start of each year."
                },
                {
                    "Popup for invasions",
                    settings.fPopupForInvasion,
                    [window](const bool b) {
                        window->setPopupForInvasion(b);
                    },
                    "Show invasion events as popups. When disabled, auto-fights."
                },
                {
                    "Popup for requests",
                    settings.fPopupForRequests,
                    [window](const bool b) {
                        window->setPopupForRequests(b);
                    },
                    "Show resource/gift requests as popups. When disabled, auto-postpones."
                },
                {
                    "Popup for tributes",
                    settings.fPopupForTributes,
                    [window](const bool b) {
                        window->setPopupForTributes(b);
                    },
                    "Show tribute demands as popups. When disabled, auto-postpones."
                },
                {
                    "Popup for troops",
                    settings.fPopupForTroops,
                    [window](const bool b) {
                        window->setPopupForTroops(b);
                    },
                    "Show troop req events as popups. When disabled, auto-postpones."
                }
            }
        },
        {
            "Sound",
            "Sound Options",
            {
                {
                    "General volume",
                    Settings::sMinVolume,
                    Settings::sMaxVolume,
                    settings.fGeneralVolume,
                    "%",
                    [](const int v) {
                        return Settings::clampVolume(v);
                    },
                    [window](const int v) {
                        window->setGeneralVolume(v);
                    }
                },
                {
                    "Music volume",
                    Settings::sMinVolume,
                    Settings::sMaxVolume,
                    settings.fMusicVolume,
                    "%",
                    [](const int v) {
                        return Settings::clampVolume(v);
                    },
                    [window](const int v) {
                        window->setMusicVolume(v);
                    }
                },
                {
                    "Voice volume",
                    Settings::sMinVolume,
                    Settings::sMaxVolume,
                    settings.fVoiceVolume,
                    "%",
                    [](const int v) {
                        return Settings::clampVolume(v);
                    },
                    [window](const int v) {
                        window->setVoiceVolume(v);
                    }
                },
                {
                    "Event volume",
                    Settings::sMinVolume,
                    Settings::sMaxVolume,
                    settings.fEventVolume,
                    "%",
                    [](const int v) {
                        return Settings::clampVolume(v);
                    },
                    [window](const int v) {
                        window->setEventVolume(v);
                    }
                },
                {
                    "Ambient volume",
                    Settings::sMinVolume,
                    Settings::sMaxVolume,
                    settings.fAmbientVolume,
                    "%",
                    [](const int v) {
                        return Settings::clampVolume(v);
                    },
                    [window](const int v) {
                        window->setAmbientVolume(v);
                    }
                }
            },
            {},
            {}
        }
    };

    if(board) {
        for(auto& page : pages) {
            if(page.fButtonLabel != "Gameplay") continue;
            const auto pid = board->personPlayer();
            page.fDifficulties.push_back({
                Language::zeusText(44, 219),
                [board, pid]() { return board->difficulty(pid); },
                [board, gw](const Difficulty d) {
                    board->setDifficulty(d);
                    if(gw) gw->rebuildGameMenu();
                }
            });
        }
    }

    return pages;
}
