#include "emainwindow.h"

#include "widgets/emainmenu.h"
#include "widgets/egraphicsmenu.h"
#include "widgets/eoptionsmenu.h"
#include "widgets/egamewidget.h"
#include "widgets/egameloadingwidget.h"
#include "widgets/egamemenu.h"
#include "widgets/emenuloadingwidget.h"
#include "widgets/eworldwidget.h"
#include "widgets/echoosegameeditmenu.h"
#include "widgets/eoptionsdata.h"
#include "widgets/eselectcolonywidget.h"
#include "widgets/etooltip.h"

#include "audio/emusic.h"
#include "audio/esounds.h"

#include "engine/ethreadpool.h"

#include "egamedir.h"

#include "fileIO/ereadstream.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <algorithm>

#include "widgets/eloadgame.h"
#include "elanguage.h"

#include "evectorhelpers.h"

#include "widgets/eeventbackground.h"

namespace {
bool writeGameSaveFile(const std::string& path,
                       const std::string& format,
                       eGameWidget* const gameWidget,
                       const stdsptr<eCampaign>& campaign) {
    const auto fsp = std::filesystem::path(path);
    const auto fspd = fsp.parent_path();
    std::filesystem::create_directories(fspd);
    std::ofstream file(path, std::ios::out | std::ios::binary |
                       std::ios::trunc);
    if(!file) return false;
    eWriteTarget target(&file);
    eWriteStream dst(target);
    dst.writeFormat(format);
    if(gameWidget) {
        const auto s = gameWidget->settings();
        s.write(dst);
    } else {
        eGameWidgetSettings s;
        s.fPaused = true;
        s.write(dst);
    }
    campaign->write(dst);
    file.close();
    return true;
}
}
#include "widgets/eepisodeintroductionwidget.h"
#include "widgets/eepisodelostwidget.h"
#include "widgets/erosterofleaders.h"

eMainWindow::eMainWindow() {}

eMainWindow::~eMainWindow() {
    if(mSdlWindow) SDL_DestroyWindow(mSdlWindow);
    if(mSdlRenderer) SDL_DestroyRenderer(mSdlRenderer);
    setWidget(nullptr);
}

bool eMainWindow::initialize(const eSettings& settings) {
    const auto& res = settings.fRes;
    const int w = res.width();
    const int h = res.height();
    const auto window = SDL_CreateWindow("eZeus",
                                         SDL_WINDOWPOS_UNDEFINED,
                                         SDL_WINDOWPOS_UNDEFINED,
                                         w, h, SDL_WINDOW_SHOWN);

    if(!window) {
        printf("Window could not be created! SDL Error: %s\n",
               SDL_GetError());
        return false;
    }
    const Uint32 flags = SDL_RENDERER_ACCELERATED/* |
                         SDL_RENDERER_PRESENTVSYNC*/;
    const auto renderer = SDL_CreateRenderer(window, -1, flags);
    if(!renderer) {
        printf("Renderer could not be created! SDL Error: %s\n",
               SDL_GetError());
        SDL_DestroyWindow(window);
        return false;
    }

    if(mSdlWindow) SDL_DestroyWindow(mSdlWindow);
    if(mSdlRenderer) SDL_DestroyRenderer(mSdlRenderer);
    mSdlWindow = window;
    mSdlRenderer = renderer;
    setResolution(res);
    setFullscreen(settings.fFullscreen);
    mSettings = settings;
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    const std::string icoPath = eGameDir::path("zeus.ico");
    const auto icon = IMG_Load(icoPath.c_str());
    SDL_SetWindowIcon(window, icon);
    eGameTextures::setSettings(mSettings);
    eMusic::setGeneralVolume(mSettings.fGeneralVolume);
    eSounds::setGeneralVolume(mSettings.fGeneralVolume);
    eMusic::setVolume(mSettings.fMusicVolume);
    eMusic::setVoiceVolume(mSettings.fVoiceVolume);
    eSounds::setVoiceVolume(mSettings.fVoiceVolume);
    eSounds::setEventVolume(mSettings.fEventVolume);
    eSounds::setAmbientVolume(mSettings.fAmbientVolume);
    return true;
}

void eMainWindow::setWidget(eWidget* const w) {
    if(mWidget) {
        if(mWidget != mGW && mWidget != mWW) {
            mWidget->deleteLater();
        }
    }
    mWidget = w;
}

eWidget* eMainWindow::takeWidget() {
    const auto w = mWidget;
    mWidget = nullptr;
    return w;
}

void eMainWindow::addSlot(const eSlot& slot) {
    mSlots.push_back(slot);
}

void eMainWindow::setResolution(const eResolution& res) {
    if(mSettings.fRes == res && !mFirstFullscrenSetting) return;
    mFirstResolutionSetting = false;
    mSettings.fRes = res;
    const int w = res.width();
    const int h = res.height();
    SDL_SetWindowSize(mSdlWindow, w, h);
}

void eMainWindow::setFullscreen(const bool f) {
    if(mSettings.fFullscreen == f && !mFirstFullscrenSetting) return;
    mFirstFullscrenSetting = false;
    mSettings.fFullscreen = f;
    SDL_SetWindowFullscreen(mSdlWindow, f ? SDL_WINDOW_FULLSCREEN : 0);
}

void eMainWindow::setKeyScrollSpeed(const int speed) {
    mSettings.fKeyScrollSpeed = eSettings::clampKeyScrollSpeed(speed);
    mSettings.write();
    if(mGW) mGW->updateKeyScrollSpeed(mSettings.fKeyScrollSpeed);
}

void eMainWindow::setDisableEdgeScroll(const bool b) {
    mSettings.fDisableEdgeScroll = b;
    mSettings.write();
}

void eMainWindow::setGameSpeed(const int speed) {
    mSettings.fGameSpeed = eSettings::clampGameSpeed(speed);
    mSettings.write();
}

void eMainWindow::setHotkey(const eHotkeyId id, const SDL_Scancode key) {
    mSettings.setHotkey(id, key);
    mSettings.write();
}

void eMainWindow::setGeneralVolume(const int volume) {
    mSettings.fGeneralVolume = eSettings::clampVolume(volume);
    eMusic::setGeneralVolume(mSettings.fGeneralVolume);
    eSounds::setGeneralVolume(mSettings.fGeneralVolume);
    mSettings.write();
}

void eMainWindow::setMusicVolume(const int volume) {
    mSettings.fMusicVolume = eSettings::clampVolume(volume);
    eMusic::setVolume(mSettings.fMusicVolume);
    mSettings.write();
}

void eMainWindow::setVoiceVolume(const int volume) {
    mSettings.fVoiceVolume = eSettings::clampVolume(volume);
    eMusic::setVoiceVolume(mSettings.fVoiceVolume);
    eSounds::setVoiceVolume(mSettings.fVoiceVolume);
    mSettings.write();
}

void eMainWindow::setEventVolume(const int volume) {
    mSettings.fEventVolume = eSettings::clampVolume(volume);
    eSounds::setEventVolume(mSettings.fEventVolume);
    mSettings.write();
}

void eMainWindow::setAmbientVolume(const int volume) {
    mSettings.fAmbientVolume = eSettings::clampVolume(volume);
    eSounds::setAmbientVolume(mSettings.fAmbientVolume);
    mSettings.write();
}

void eMainWindow::setWarehouseDefaultAcceptNone(const bool b) {
    mSettings.fWarehouseDefaultAcceptNone = b;
    mSettings.write();
}

void eMainWindow::setDoubleCartCapacity(const bool b) {
    mSettings.fDoubleCartCapacity = b;
    mSettings.write();
}

void eMainWindow::setAgorasTakeFromTradingPosts(const bool b) {
    mSettings.fAgorasTakeFromTradingPosts = b;
    mSettings.write();
    if (mBoard) mBoard->setAgorasTakeFromTradingPosts(b);
}

void eMainWindow::setEnableYearlyAutosaves(const bool b) {
    mSettings.fEnableYearlyAutosaves = b;
    mSettings.write();
}

void eMainWindow::setLastDifficulty(const eDifficulty d) {
    mSettings.fLastDifficulty = d;
    mSettings.write();
}

void eMainWindow::startGameAction(eGameBoard* const board,
                                  const eGameWidgetSettings& settings) {
    const auto show = [this, board, settings]() {
        showGame(board, settings);
    };
    startGameAction(show);
}

void eMainWindow::startGameAction(const stdsptr<eCampaign>& c,
                                  const eGameWidgetSettings& settings) {
    const auto show = [this, c, settings]() {
        showGame(c, settings);
    };
    startGameAction(show);
}

void eMainWindow::startGameAction(const eAction& a) {
    clearWidgets();
    const auto l = new eGameLoadingWidget(this);
    l->resize(width(), height());
    l->setDoneAction(a);
    setWidget(l);
    l->initialize();
}

void eMainWindow::showEpisodeIntroduction(
        const stdsptr<eCampaign>& c) {
    clearWidgets();
    if(c) mCampaign = c;
    const auto e = new eEpisodeIntroductionWidget(this);
    const auto proceedA = [this]() {
        mCampaign->startEpisode();
        const auto dir = leaderSaveDir();
        saveGame(dir + "autosave replay.ez2");
        startGameAction([this]() {
            eGameWidgetSettings settings;
            settings.fPaused = true;
            showGame(mCampaign, settings);
        });
    };
    e->resize(width(), height());
    const auto ee = mCampaign->currentEpisode();

    const auto path = mCampaign->currentEpisodeAudioFilePath(true);
    const bool played = eMusic::playCampaignVoice(path);
    if(!played) eMusic::playMissionIntroMusic();

    e->initialize(mCampaign,
                  mCampaign->titleText(),
                  ee->fIntroduction,
                  ee->fGoals,
                  proceedA,
                  eEpisodeIntroType::intro);
    setWidget(e);
}

std::string eMainWindow::leaderSaveDir() const {
    return eGameDir::saveDir() + mLeader + "/";
}

std::string eMainWindow::mostRecentSavePath() const {
    const auto folder = leaderSaveDir();
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

void eMainWindow::clearWidgets() {
    if(mGW && mWidget != mGW) {
        mGW->setBoard(nullptr);
        mGW->deleteLater();
        mGW = nullptr;
    }
    if(mWW && mWidget != mWW) {
        mWW->deleteLater();
        mWW = nullptr;
    }
}

void eMainWindow::episodeFinished() {
    clearWidgets();
    if(!mCampaign) return;
    mCampaign->episodeFinished();
    const bool f = mCampaign->finished();
    if(f) return adventureComplete();
    const auto n = mCampaign->currentEpisodeType();
    if(n == eEpisodeType::parentCity) {
        showEpisodeIntroduction();
    } else {
        const auto w = new eSelectColonyWidget(this);
        const auto sel = mCampaign->remainingColonies();
        const auto selA = [this](const stdsptr<eWorldCity>& c) {
            int cid = 0;
            const auto& eps = mCampaign->colonyEpisodes();
            for(const auto& e : eps) {
                if(e->fCity == c) break;
                cid++;
            }
            mCampaign->setCurrentColonyEpisode(cid);
            showEpisodeIntroduction();
        };
        w->resize(width(), height());
        w->initialize(sel, selA, &mCampaign->worldBoard());
        setWidget(w);
    }
}

void eMainWindow::adventureComplete() {
    clearWidgets();
    if(!mCampaign) return;
    const auto e = new eEpisodeIntroductionWidget(this);
    const auto proceedA = [this]() {
        showMainMenu();
    };
    e->resize(width(), height());

    const auto path = mCampaign->adventureVictoryAudioFilePath();
    const bool played = eMusic::playCampaignVoice(path);
    if(!played) eMusic::playCampaignVictoryMusic();

    e->initialize(mCampaign,
                  eLanguage::zeusText(62, 0),
                  mCampaign->completeText(),
                  {},
                  proceedA,
                  eEpisodeIntroType::campaingVictory);
    setWidget(e);
}

void eMainWindow::episodeLost() {
    clearWidgets();
    const auto e = new eEpisodeLostWidget(this);
    const auto proceedA = [this]() {
        showMainMenu();
    };
    e->resize(width(), height());
    e->initialize(proceedA);
    setWidget(e);
}

bool eMainWindow::saveGame(const std::string& path) {
    auto ez2Path = std::filesystem::path(path);
    ez2Path.replace_extension(".ez2");
    return writeGameSaveFile(ez2Path.string(), "eZeus.ez2", mGW, mCampaign);
}

bool eMainWindow::loadGame(const std::string& path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if(!file) return false;
    eReadSource source(&file);
    eReadStream src(source);
    src.readFormat();
    const auto& format = src.format();
    if(format != "eZeus.ez2") {
        printf("Invalid file '%s' format '%s', expected 'eZeus.ez2'.\n",
               path.c_str(), format.c_str());
        return false;
    }
    eGameWidgetSettings s;
    s.read(src);
    const auto c = std::make_shared<eCampaign>();
    c->read(src);
    c->loadStrings();
    c->loadNumbers();
    src.handlePostFuncs();
    file.close();

    startGameAction(c, s);
    return true;
}

void eMainWindow::closeGame() {
    if(!mGW) return;
    if(mGW) {
        mGW->setBoard(nullptr);
        mGW->deleteLater();
        mGW = nullptr;
    }
    if(mWW) {
        mWW->deleteLater();
        mWW = nullptr;
    }
    showMainMenu();
}

void eMainWindow::showRosterOfLeaders() {
    clearWidgets();
    eMusic::playMenuMusic();
    const auto rol = new eRosterOfLeaders(this);
    rol->resize(width(), height());
    rol->initialize();
    setWidget(rol);
}

void eMainWindow::showMenuLoading() {
    const auto mlw = new eMenuLoadingWidget(this);
    mlw->setDoneAction([this]() {
        const auto ls = eRosterOfLeaders::sLeaders();
        if(ls.size() == 1) setLeader(ls[0]);
        if(mLeader.empty()) {
            showRosterOfLeaders();
        } else {
            showMainMenu();
        }
    });
    mlw->initialize();
    mlw->resize(width(), height());
    setWidget(mlw);
}

void eMainWindow::showMainMenu() {
    mCampaign = nullptr;
    clearWidgets();
    eMusic::playMenuMusic();

    const auto mm = new eMainMenu(this);
    mm->resize(width(), height());
    setWidget(mm);

    const auto newGameAction = [this]() {
        showChooseGameMenu();
    };

    const auto recentSave = mostRecentSavePath();
    const auto continueGameAction = [this]() {
        const auto path = mostRecentSavePath();
        if(path.empty()) return;
        loadGame(path);
    };

    const auto loadGameAction = [this]() {
        const auto fw = new eLoadGame(this);
        const auto func = [this](const std::string& path) {
            return loadGame(path);
        };
        const auto closeAct = [fw]() {
            fw->deleteLater();
        };
        const auto dir = leaderSaveDir();
        fw->intialize(eLanguage::zeusText(1, 3),
                      dir, func, closeAct);
        execDialog(fw, true, closeAct);
    };

    const auto editGameAction = [this]() {
        showChooseGameEditMenu();
    };

    const auto settingsAction = [this]() {
        showSettingsMenu();
    };

    const auto optionsAction = [this]() {
        showOptionsMenu();
    };

    const auto quitAction = [this]() {
        mQuit = true;
    };

    const auto leaderAction = [this]() {
        showRosterOfLeaders();
    };

    mm->initialize(continueGameAction,
                   !recentSave.empty(),
                   newGameAction,
                   loadGameAction,
                   editGameAction,
                   settingsAction,
                   optionsAction,
                   quitAction,
                   leaderAction);
}

void eMainWindow::applyGraphicsSettings(const eSettings& settings) {
    const bool loadNeeded = settings.fRes != mSettings.fRes;
    setResolution(settings.fRes);
    setFullscreen(settings.fFullscreen);
    mSettings = settings;
    mSettings.write();
    if(!mSettings.fTinyTextures &&
       !mSettings.fSmallTextures &&
       !mSettings.fMediumTextures &&
       !mSettings.fLargeTextures) {
        mSettings.fSmallTextures = true;
    }
    eGameTextures::setSettings(mSettings);
    if(loadNeeded) showMenuLoading();
}

void eMainWindow::showSettingsMenu() {
    const auto esm = new eGraphicsMenu(mSettings, this);
    esm->resize(width(), height());

    const auto applyA = [this](const eSettings& settings) {
        const bool loadNeeded = settings.fRes != mSettings.fRes;
        applyGraphicsSettings(settings);
        if(!loadNeeded) showMainMenu();
    };
    const auto fullscrennA = [this](const bool f) {
        setFullscreen(f);
    };
    esm->initialize(applyA, fullscrennA);
    execDialog(esm, true, [this]() { showMainMenu(); });
}

void eMainWindow::showOptionsMenu() {
    const auto d = new eOptionsMenu(getOptionsPages(this), this);
    d->initialize();
    execDialog(d, true, [this]() { showMainMenu(); });
}

void eMainWindow::showChooseGameMenu() {
    const auto gem = new eChooseGameEditMenu(this);
    gem->resize(width(), height());
    gem->initialize(false);
    setWidget(gem);
}

void eMainWindow::showChooseGameEditMenu() {
    const auto gem = new eChooseGameEditMenu(this);
    gem->resize(width(), height());
    gem->initialize(true);
    setWidget(gem);
}

void eMainWindow::showGame(const stdsptr<eCampaign>& c,
                           const eGameWidgetSettings& settings) {
    mCampaign = c;
    const auto e = c->currentEpisode();
    showGame(e->fBoard, settings);
}

void eMainWindow::showGame(eGameBoard* b,
                           const eGameWidgetSettings& settings) {
    if(!b) b = mBoard;

    if(b == mBoard && mGW) {
        return setWidget(mGW);
    }

    if(mGW) {
        mGW->setBoard(nullptr);
        mGW->deleteLater();
        mGW = nullptr;
    }

    mBoard = b;
    if(mBoard) mBoard->setDoubleCartCapacity(mSettings.fDoubleCartCapacity);
    if(mBoard) mBoard->setAgorasTakeFromTradingPosts(mSettings.fAgorasTakeFromTradingPosts);

    eMusic::playRandomMusic();
    mGW = new eGameWidget(this);
    mGW->setBoard(b);
    mGW->resize(width(), height());
    mGW->initialize();
    mGW->setSettings(settings);
    setWidget(mGW);
}

void eMainWindow::showWorld() {
    if(mWidget == mWW) return;
    if(!mCampaign) return;
    if(!mWW) {
        mWW = new eWorldWidget(this);
        mWW->resize(width(), height());
        mWW->initialize();
        mWW->setBoard(mBoard);
    } else {
        mWW->update();
    }
    setWidget(mWW);
}

void eMainWindow::execDialog(
        eWidget* const d, const bool closable,
        const eAction &closeFunc,
        eWidget* const parent) {
    if(!mWidget) return;
    const auto bg = new eEventBackground(this);
    if(closeFunc) {
        bg->initialize(parent ? parent : mWidget, d, closable, closeFunc);
    } else {
        const auto closeFunc = [d]() {
            d->deleteLater();
        };
        bg->initialize(parent ? parent : mWidget, d, closable, closeFunc);
    }
}

int eMainWindow::exec() {
    using namespace std::chrono;
    using namespace std::chrono_literals;

    showMenuLoading();

    eMouseButton button{eMouseButton::none};
    eMouseButton buttons{eMouseButton::none};

    SDL_Event e;
    eTooltip tooltip(*this);

    const bool showFPS = false;
    const double fpsClamp = kFpsClamp;

    int c = 0;
    int fpsVal = 0;
    bool resetRenderTargets = false;
    while(!mQuit) {
        const auto fpsStart = high_resolution_clock::now();

        while(SDL_PollEvent(&e)) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            const bool shift = mShiftPressed > 0;
            const bool ctrl = mCtrlPressed > 0;
            if(e.type == SDL_QUIT) {
                mQuit = true;
            } else if(e.type == SDL_WINDOWEVENT) {
                const auto we = e.window.event;
                if(we == SDL_WINDOWEVENT_MINIMIZED) {
                    resetRenderTargets = true;
                    while(SDL_WaitEvent(&e)) {
                        if(e.window.event == SDL_WINDOWEVENT_RESTORED) {
                            break;
                        }
                    }
                } else if(we == SDL_WINDOWEVENT_EXPOSED) {
                    resetRenderTargets = true;
                }
            } else if(e.type == SDL_RENDER_TARGETS_RESET ||
                      e.type == SDL_RENDER_DEVICE_RESET) {
                resetRenderTargets = true;
            } else if(e.type == SDL_MOUSEMOTION) {
                const eMouseEvent me(x, y, shift, ctrl, buttons, button);
                if(mWidget) mWidget->mouseMove(me);
            } else if(e.type == SDL_MOUSEBUTTONDOWN) {
                switch(e.button.button) {
                case SDL_BUTTON_LEFT:
                    button = eMouseButton::left;
                    break;
                case SDL_BUTTON_RIGHT:
                    button = eMouseButton::right;
                    break;
                case SDL_BUTTON_MIDDLE:
                    button = eMouseButton::middle;
                    break;
                default: continue;
                }
                buttons = button | buttons;

                const eMouseEvent me(x, y, shift, ctrl, buttons, button);
                if(mWidget) mWidget->mousePress(me);
            } else if(e.type == SDL_MOUSEBUTTONUP) {
                switch(e.button.button) {
                case SDL_BUTTON_LEFT:
                    button = eMouseButton::left;
                    break;
                case SDL_BUTTON_RIGHT:
                    button = eMouseButton::right;
                    break;
                case SDL_BUTTON_MIDDLE:
                    button = eMouseButton::middle;
                    break;
                default: continue;
                }
                buttons = buttons & ~button;
                const eMouseEvent me(x, y, shift, ctrl, buttons, button);
                if(mWidget) mWidget->mouseRelease(me);
            } else if(e.type == SDL_MOUSEWHEEL) {
                const eMouseWheelEvent me(x, y, shift, ctrl, buttons, e.wheel.y);
                if(mWidget) mWidget->mouseWheel(me);
            } else if(e.type == SDL_KEYDOWN) {
                const auto k = e.key.keysym.scancode;
                if(k == SDL_Scancode::SDL_SCANCODE_LSHIFT ||
                   k == SDL_Scancode::SDL_SCANCODE_RSHIFT) {
                    mShiftPressed++;
                } else if(k == SDL_Scancode::SDL_SCANCODE_LCTRL ||
                          k == SDL_Scancode::SDL_SCANCODE_RCTRL) {
                    mCtrlPressed++;
                }
                const eKeyPressEvent ke(x, y, shift, ctrl, buttons, k);
                if(mWidget) mWidget->keyPress(ke);
            } else if(e.type == SDL_KEYUP) {
                const auto k = e.key.keysym.scancode;
                if(k == SDL_Scancode::SDL_SCANCODE_LSHIFT ||
                   k == SDL_Scancode::SDL_SCANCODE_RSHIFT) {
                    mShiftPressed--;
                } else if(k == SDL_Scancode::SDL_SCANCODE_LCTRL ||
                          k == SDL_Scancode::SDL_SCANCODE_RCTRL) {
                    mCtrlPressed--;
                }
                const eKeyPressEvent ke(x, y, shift, ctrl, buttons, k);
                if(mWidget) mWidget->keyRelease(ke);
            }
        }

        if(resetRenderTargets) {
            resetRenderTargets = false;
            if(mWidget) mWidget->renderTargetsReset();
        }

        SDL_SetRenderDrawColor(mSdlRenderer, 0x0, 0x0, 0x0, 0xFF);
        SDL_RenderClear(mSdlRenderer);

        ePainter p(mSdlRenderer);

        eMusic::incTime();
        if(mWidget) {
            mWidget->paint(p);
            tooltip.update();
            if(!tooltip.empty()) {
                const auto& res = resolution();
                const int pp = 25*res.multiplier();
                const int wtt = tooltip.width();
                const int htt = tooltip.height();
                int mx, my;
                SDL_GetMouseState(&mx, &my);
                int xtt;
                int ytt;
                if(mx > width()/2) {
                    xtt = mx - wtt;
                } else {
                    xtt = mx;
                }
                if(my > height()/2) {
                    ytt = my - htt - pp;
                } else {
                    ytt = my + pp;
                }
                tooltip.paint(xtt, ytt, p);
            }
        }

        if(showFPS) {
            p.setFont(eFonts::defaultFont(resolution()));
            p.drawText(0, 0, std::to_string(fpsVal), eFontColor::dark);
        }

        SDL_RenderPresent(mSdlRenderer);

        std::vector<eSlot> slots;
        std::swap(slots, mSlots);
        for(const auto& s : slots) {
            s();
        }

        const auto fpsEnd = high_resolution_clock::now();
        const duration<double, std::milli> fpsElapsed = fpsEnd - fpsStart;
        const duration<double, std::milli> fpsDuration(1000./fpsClamp);
        const duration<double, std::milli> fpsSleep(fpsDuration - fpsElapsed);
        std::this_thread::sleep_for(fpsSleep);

        if(showFPS) {
            c++;
            if(c % 25 == 0) {
                const auto fpsEnd = high_resolution_clock::now();
                const duration<double, std::milli> fpsElapsed = fpsEnd - fpsStart;
                fpsVal = (int)std::round(1000./fpsElapsed.count());
            }
        }
    }

    return 0;
}
