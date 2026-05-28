#include "eoptionsdata.h"

#include "emainwindow.h"
#include "esettings.h"
#include "elanguage.h"
#include "engine/e-game-board.h"
#include "widgets/game-widget.h"

std::vector<eOptionsMenu::ePage> getOptionsPages(eMainWindow* const window,
                                                 GameBoard* const board,
                                                 GameWidget* const gw) {
    const auto& settings = window->settings();
    std::vector<eOptionsMenu::ePage> pages = {
        {
            "General",
            "General Options",
            {
                {
                    "Key scroll speed",
                    eSettings::sMinKeyScrollSpeed,
                    eSettings::sMaxKeyScrollSpeed,
                    settings.fKeyScrollSpeed,
                    "",
                    [](const int v) {
                        return eSettings::clampKeyScrollSpeed(v);
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
            "Hotkeys",
            "Hotkeys",
            {},
            {
                {"Game menu", eHotkeyId::gameMenu, settings.fHotkeyGameMenu,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Pause game", eHotkeyId::pause, settings.fHotkeyPause,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"--- Speed Control ---", eHotkeyId::speedUp, settings.fHotkeySpeedUp, nullptr},
                {"Increase game speed", eHotkeyId::speedUp, settings.fHotkeySpeedUp,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Decrease game speed", eHotkeyId::speedDown, settings.fHotkeySpeedDown,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"--- Building Tools ---", eHotkeyId::rotatePreview, settings.fHotkeyRotatePreview, nullptr},
                {"Rotate building preview", eHotkeyId::rotatePreview, settings.fHotkeyRotatePreview,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Copy hovered building mode", eHotkeyId::copyBuilding, settings.fHotkeyCopyBuilding,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Delete tool", eHotkeyId::deleteTool, settings.fHotkeyDeleteTool,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Repair tool", eHotkeyId::repairTool, settings.fHotkeyRepairTool,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Undo last action", eHotkeyId::undo, settings.fHotkeyUndo,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"--- Construction ---", eHotkeyId::buildRoad, settings.fHotkeyBuildRoad, nullptr},
                {"Build road", eHotkeyId::buildRoad, settings.fHotkeyBuildRoad,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Roadblock", eHotkeyId::buildRoadblock, settings.fHotkeyBuildRoadblock,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Maintenance office", eHotkeyId::buildMaintenanceOffice, settings.fHotkeyBuildMaintenanceOffice,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Common housing", eHotkeyId::buildCommonHousing, settings.fHotkeyBuildCommonHousing,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Watchpost", eHotkeyId::buildWatchpost, settings.fHotkeyBuildWatchpost,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Stamp tool", eHotkeyId::buildStamp, settings.fHotkeyBuildStamp,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Display stamp manager", eHotkeyId::stampManager, settings.fHotkeyStampManager,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Show roads overlay", eHotkeyId::showRoadsOverlay, settings.fHotkeyShowRoadsOverlay,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"--- Camera ---", eHotkeyId::scrollLeft, settings.fHotkeyScrollLeft, nullptr},
                {"Smooth scroll left", eHotkeyId::scrollLeft, settings.fHotkeyScrollLeft,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Smooth scroll right", eHotkeyId::scrollRight, settings.fHotkeyScrollRight,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Smooth scroll up", eHotkeyId::scrollUp, settings.fHotkeyScrollUp,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Smooth scroll down", eHotkeyId::scrollDown, settings.fHotkeyScrollDown,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"--- Bookmarks ---", eHotkeyId::bookmark1, settings.fHotkeyBookmark1, nullptr},
                {"Bookmark 1", eHotkeyId::bookmark1, settings.fHotkeyBookmark1,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Bookmark 2", eHotkeyId::bookmark2, settings.fHotkeyBookmark2,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Bookmark 3", eHotkeyId::bookmark3, settings.fHotkeyBookmark3,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Bookmark 4", eHotkeyId::bookmark4, settings.fHotkeyBookmark4,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"--- Menu Tabs ---", eHotkeyId::menuTab1, settings.fHotkeyMenuTab1, nullptr},
                {"Population tab", eHotkeyId::menuTab1, settings.fHotkeyMenuTab1,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Husbandry tab", eHotkeyId::menuTab2, settings.fHotkeyMenuTab2,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Industry tab", eHotkeyId::menuTab3, settings.fHotkeyMenuTab3,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Distribution tab", eHotkeyId::menuTab4, settings.fHotkeyMenuTab4,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Hygiene & safety tab", eHotkeyId::menuTab5, settings.fHotkeyMenuTab5,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Administration tab", eHotkeyId::menuTab6, settings.fHotkeyMenuTab6,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Culture tab", eHotkeyId::menuTab7, settings.fHotkeyMenuTab7,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Mythology tab", eHotkeyId::menuTab8, settings.fHotkeyMenuTab8,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Military tab", eHotkeyId::menuTab9, settings.fHotkeyMenuTab9,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Aesthetics tab", eHotkeyId::menuTab10, settings.fHotkeyMenuTab10,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
                     window->setHotkey(id, key);
                 }},
                {"Overview tab", eHotkeyId::menuTab11, settings.fHotkeyMenuTab11,
                 [window](const eHotkeyId id, const SDL_Scancode key) {
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
                    eSettings::sMinVolume,
                    eSettings::sMaxVolume,
                    settings.fGeneralVolume,
                    "%",
                    [](const int v) {
                        return eSettings::clampVolume(v);
                    },
                    [window](const int v) {
                        window->setGeneralVolume(v);
                    }
                },
                {
                    "Music volume",
                    eSettings::sMinVolume,
                    eSettings::sMaxVolume,
                    settings.fMusicVolume,
                    "%",
                    [](const int v) {
                        return eSettings::clampVolume(v);
                    },
                    [window](const int v) {
                        window->setMusicVolume(v);
                    }
                },
                {
                    "Voice volume",
                    eSettings::sMinVolume,
                    eSettings::sMaxVolume,
                    settings.fVoiceVolume,
                    "%",
                    [](const int v) {
                        return eSettings::clampVolume(v);
                    },
                    [window](const int v) {
                        window->setVoiceVolume(v);
                    }
                },
                {
                    "Event volume",
                    eSettings::sMinVolume,
                    eSettings::sMaxVolume,
                    settings.fEventVolume,
                    "%",
                    [](const int v) {
                        return eSettings::clampVolume(v);
                    },
                    [window](const int v) {
                        window->setEventVolume(v);
                    }
                },
                {
                    "Ambient volume",
                    eSettings::sMinVolume,
                    eSettings::sMaxVolume,
                    settings.fAmbientVolume,
                    "%",
                    [](const int v) {
                        return eSettings::clampVolume(v);
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
                eLanguage::zeusText(44, 219),
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
