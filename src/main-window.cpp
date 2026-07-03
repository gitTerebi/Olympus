#include "main-window.h"

#include "widgets/emainmenu.h"
#include "widgets/eoptionsmenu.h"
#include "widgets/game-widget.h"
#include "widgets/egameloadingwidget.h"
#include "widgets/game-menu.h"
#include "widgets/emenuloadingwidget.h"
#include "widgets/eworldwidget.h"
#include "widgets/echoosegameeditmenu.h"
#include "widgets/eoptionsdata.h"
#include "widgets/eselectcolonywidget.h"
#include "widgets/etooltip.h"

#include "audio/music.h"
#include "audio/sounds.h"

#include "engine/ethreadpool.h"

#include "game-dir.h"

#include "fileIO/read-stream.h"
#include "fileIO/save-archive.h"

#include <chrono>
#include <thread>
#include <filesystem>
#include <fstream>
#include <algorithm>

#include "widgets/eloadgame.h"
#include "language.h"

#include "vector-helpers.h"

#include "widgets/eeventbackground.h"

#include "widgets/paint/world-postprocess-shader.h"

namespace {
bool writeGameSaveFile(const std::string& path,
                       const std::string& format,
                       GameWidget* const gameWidget,
                       const stdsptr<eCampaign>& campaign) {
    const auto fsp = std::filesystem::path(path);
    const auto fspd = fsp.parent_path();
    std::filesystem::create_directories(fspd);
    std::ofstream file(path, std::ios::out | std::ios::binary |
                       std::ios::trunc);
    if(!file) return false;
    WriteTarget target(&file);
    WriteStream dst(target);
    dst.writeFormat(format);
    if(gameWidget) {
        auto s = gameWidget->settings();
        SaveArchive settingsAr(dst);
        s.serialize(settingsAr);
    } else {
        GameWidgetSettings s;
        s.fPaused = true;
        SaveArchive settingsAr(dst);
        s.serialize(settingsAr);
    }
    {
        SaveArchive campaignAr(dst);
        campaign->serialize(campaignAr);
    }
    file.close();
    return true;
}
}
#include "widgets/eepisodeintroductionwidget.h"
#include "widgets/eepisodelostwidget.h"
#include "widgets/erosterofleaders.h"

MainWindow::MainWindow() {}

MainWindow::~MainWindow() {
    setWidget(nullptr);
    clearWidgets();
    while(!mSlots.empty()) {
        std::vector<eSlot> slots;
        std::swap(slots, mSlots);
        for(const auto& s : slots) {
            s();
        }
    }
    if(mSdlRenderer) SDL_DestroyRenderer(mSdlRenderer);
    if(mSdlWindow) SDL_DestroyWindow(mSdlWindow);
}

bool MainWindow::initialize(const Settings& settings, const bool offscreen) {
    const auto& res = settings.fRes;
    const int w = res.width();
    const int h = res.height();
    const int windowX = offscreen ? -32000 : SDL_WINDOWPOS_UNDEFINED;
    const int windowY = offscreen ? -32000 : SDL_WINDOWPOS_UNDEFINED;
    // Resizable so the user can maximize the window past the configured resolution;
    // the bicubic pass upscales the fixed-resolution frame to fill it.
    const auto window = SDL_CreateWindow("eZeus",
                                         windowX,
                                         windowY,
                                         w, h,
                                         SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if(!window) {
        printf("Window could not be created! SDL Error: %s\n",
               SDL_GetError());
        return false;
    }
    const Uint32 flags = SDL_RENDERER_ACCELERATED |
                         SDL_RENDERER_PRESENTVSYNC;
    const auto renderer = SDL_CreateRenderer(window, -1, flags);
    if(!renderer) {
        printf("Renderer could not be created! SDL Error: %s\n",
               SDL_GetError());
        SDL_DestroyWindow(window);
        return false;
    }
    SDL_RendererInfo info;
    if(SDL_GetRendererInfo(renderer, &info) == 0) {
        printf("Renderer: %s flags=0x%x\n", info.name, info.flags);
    }

    if(mSdlWindow) SDL_DestroyWindow(mSdlWindow);
    if(mSdlRenderer) SDL_DestroyRenderer(mSdlRenderer);
    mSdlWindow = window;
    mSdlRenderer = renderer;
    setResolution(res);
    setDisplayMode(settings.fDisplayMode);
    mSettings = settings;
    applyPostprocessFilters();
    if(offscreen) {
        SDL_SetWindowPosition(window, -32000, -32000);
    } else {
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }

    const std::string icoPath = GameDir::path("zeus.ico");
    const auto icon = IMG_Load(icoPath.c_str());
    SDL_SetWindowIcon(window, icon);
    GameTextures::setSettings(mSettings);
    eMusic::setGeneralVolume(mSettings.fGeneralVolume);
    eSounds::setGeneralVolume(mSettings.fGeneralVolume);
    eMusic::setVolume(mSettings.fMusicVolume);
    eMusic::setVoiceVolume(mSettings.fVoiceVolume);
    eSounds::setVoiceVolume(mSettings.fVoiceVolume);
    eSounds::setEventVolume(mSettings.fEventVolume);
    eSounds::setAmbientVolume(mSettings.fAmbientVolume);
    return true;
}

int MainWindow::width() const {
    int w = 0;
    int h = 0;
    if(mSdlWindow) {
        SDL_GetWindowSize(mSdlWindow, &w, &h);
    }
    if(w > 0) {
        return w;
    }
    return resolution().width();
}

int MainWindow::height() const {
    int w = 0;
    int h = 0;
    if(mSdlWindow) {
        SDL_GetWindowSize(mSdlWindow, &w, &h);
    }
    if(h > 0) {
        return h;
    }
    return resolution().height();
}

void MainWindow::setWidget(eWidget* const w) {
    if(mWidget) {
        if(mWidget != mGW && mWidget != mWW) {
            mWidget->deleteLater();
        }
    }
    mWidget = w;
}

eWidget* MainWindow::takeWidget() {
    const auto w = mWidget;
    mWidget = nullptr;
    return w;
}

void MainWindow::addSlot(const eSlot& slot) {
    mSlots.push_back(slot);
}

void MainWindow::mapWindowToFrame(int& x, int& y) const {
    (void)x;
    (void)y;
}

bool MainWindow::resizeCurrentWidgetToWindow() {
    int w = 0;
    int h = 0;
    SDL_GetWindowSize(mSdlWindow, &w, &h);
    if(!mWidget || w <= 0 || h <= 0) return false;
    mWidget->resize(w, h);
    mWidget->windowSizeChanged(w, h);
    return true;
}

bool MainWindow::isWindowMaximized() const {
    return mSdlWindow &&
           (SDL_GetWindowFlags(mSdlWindow) & SDL_WINDOW_MAXIMIZED) != 0;
}

void MainWindow::setResolution(const eResolution& res) {
    if(mSettings.fRes == res && !mFirstResolutionSetting) return;
    mFirstResolutionSetting = false;
    mSettings.fRes = res;
    const int w = res.width();
    const int h = res.height();
    if(mSettings.fDisplayMode == DisplayMode::window && isWindowMaximized()) {
        printf("Keeping maximized window size while changing resolution\n");
    } else {
        SDL_SetWindowSize(mSdlWindow, w, h);
    }
    resizeCurrentWidgetToWindow();
}

void MainWindow::setResolution(const int resolution) {
    if(resolution < 0 ||
       resolution >= static_cast<int>(eResolution::sResolutions.size())) {
        return;
    }
    auto settings = mSettings;
    const auto& r = eResolution::sResolutions[resolution];
    settings.fRes = eResolution(r.width(), r.height(), mSettings.fUiScale);
    applyGraphicsSettings(settings);
}

void MainWindow::setDisplayMode(const DisplayMode mode) {
    if(mSettings.fDisplayMode == mode && !mFirstDisplayModeSetting) return;
    mFirstDisplayModeSetting = false;
    mSettings.fDisplayMode = mode;
    Uint32 flags = 0;
    if(mode == DisplayMode::fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    } else if(mode == DisplayMode::borderless) {
        flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    SDL_SetWindowFullscreen(mSdlWindow, flags);
    resizeCurrentWidgetToWindow();
}

void MainWindow::applyPostprocessFilters() {
    setPostprocessFilters(static_cast<int>(mSettings.fInterpolation),
                          static_cast<int>(mSettings.fUpscale),
                          mSettings.fUpscaleFactor);
}

void MainWindow::setInterpolation(const int interpolation) {
    mSettings.fInterpolation = static_cast<Interpolation>(interpolation);
    applyPostprocessFilters();
    mSettings.write();
}

void MainWindow::setUpscale(const int upscale) {
    mSettings.fUpscale = static_cast<Upscale>(upscale);
    applyPostprocessFilters();
    mSettings.write();
}

void MainWindow::setUpscaleFactor(const int factor) {
    mSettings.fUpscaleFactor = factor < 2 ? 2 : (factor > 6 ? 6 : factor);
    applyPostprocessFilters();
    mSettings.write();
}

void MainWindow::setUiScale(const int scale) {
    const int clamped = scale < 0 ? 0 :
        (scale > static_cast<int>(eUIScale::large) ?
             static_cast<int>(eUIScale::large) : scale);
    const auto oldScale = mSettings.fUiScale;
    mSettings.fUiScale = static_cast<eUIScale>(clamped);
    mSettings.fRes = eResolution(mSettings.fRes.width(),
                                 mSettings.fRes.height(),
                                 mSettings.fUiScale);
    mSettings.write();
    if(mSettings.fUiScale != oldScale && mGW) {
        mGW->reloadUiPreservingOverlays();
    }
}

void MainWindow::setTopSidebarScale(const int scale) {
    const int clamped = Settings::clampTopSidebarScale(scale);
    if(mSettings.fTopSidebarScale == clamped) return;
    mSettings.fTopSidebarScale = clamped;
    mSettings.write();
    if(mGW) {
        mGW->reloadUiPreservingOverlays();
    }
}

void MainWindow::setDisplayMode(const int mode) {
    if(mode < 0 || mode >= static_cast<int>(DisplayMode::count)) return;
    setDisplayMode(static_cast<DisplayMode>(mode));
    mSettings.write();
}

void MainWindow::setKeyScrollSpeed(const int speed) {
    mSettings.fKeyScrollSpeed = Settings::clampKeyScrollSpeed(speed);
    mSettings.write();
    if(mGW) mGW->updateKeyScrollSpeed(mSettings.fKeyScrollSpeed);
}

void MainWindow::setDisableEdgeScroll(const bool b) {
    mSettings.fDisableEdgeScroll = b;
    mSettings.write();
}

void MainWindow::setGameSpeed(const int speed) {
    mSettings.fGameSpeed = Settings::clampGameSpeed(speed);
    mSettings.write();
}

void MainWindow::setHotkey(const HotkeyId id, const SDL_Scancode key) {
    mSettings.setHotkey(id, key);
    mSettings.write();
}

void MainWindow::setGeneralVolume(const int volume) {
    mSettings.fGeneralVolume = Settings::clampVolume(volume);
    eMusic::setGeneralVolume(mSettings.fGeneralVolume);
    eSounds::setGeneralVolume(mSettings.fGeneralVolume);
    mSettings.write();
}

void MainWindow::setMusicVolume(const int volume) {
    mSettings.fMusicVolume = Settings::clampVolume(volume);
    eMusic::setVolume(mSettings.fMusicVolume);
    mSettings.write();
}

void MainWindow::setVoiceVolume(const int volume) {
    mSettings.fVoiceVolume = Settings::clampVolume(volume);
    eMusic::setVoiceVolume(mSettings.fVoiceVolume);
    eSounds::setVoiceVolume(mSettings.fVoiceVolume);
    mSettings.write();
}

void MainWindow::setEventVolume(const int volume) {
    mSettings.fEventVolume = Settings::clampVolume(volume);
    eSounds::setEventVolume(mSettings.fEventVolume);
    mSettings.write();
}

void MainWindow::setAmbientVolume(const int volume) {
    mSettings.fAmbientVolume = Settings::clampVolume(volume);
    eSounds::setAmbientVolume(mSettings.fAmbientVolume);
    mSettings.write();
}

void MainWindow::setWarehouseDefaultAcceptNone(const bool b) {
    mSettings.fWarehouseDefaultAcceptNone = b;
    mSettings.write();
}

void MainWindow::setDoubleCartCapacity(const bool b) {
    mSettings.fDoubleCartCapacity = b;
    mSettings.write();
}

void MainWindow::setAgorasTakeFromTradingPosts(const bool b) {
    mSettings.fAgorasTakeFromTradingPosts = b;
    mSettings.write();
    if (mBoard) mBoard->setAgorasTakeFromTradingPosts(b);
}

void MainWindow::setEnableYearlyAutosaves(const bool b) {
    mSettings.fEnableYearlyAutosaves = b;
    mSettings.write();
}

void MainWindow::setPopupForInvasion(const bool b) {
    mSettings.fPopupForInvasion = b;
    mSettings.write();
}

void MainWindow::setPopupForRequests(const bool b) {
    mSettings.fPopupForRequests = b;
    mSettings.write();
}

void MainWindow::setPopupForTributes(const bool b) {
    mSettings.fPopupForTributes = b;
    mSettings.write();
}

void MainWindow::setPopupForTroops(const bool b) {
    mSettings.fPopupForTroops = b;
    mSettings.write();
}

void MainWindow::setLastDifficulty(const Difficulty d) {
    mSettings.fLastDifficulty = d;
    mSettings.write();
}

void MainWindow::startGameAction(GameBoard* const board,
                                  const GameWidgetSettings& settings) {
    const auto show = [this, board, settings]() {
        showGame(board, settings);
    };
    startGameAction(show);
}

void MainWindow::startGameAction(const stdsptr<eCampaign>& c,
                                  const GameWidgetSettings& settings) {
    const auto show = [this, c, settings]() {
        showGame(c, settings);
    };
    startGameAction(show);
}

void MainWindow::startGameAction(const eAction& a) {
    clearWidgets();
    const auto l = new eGameLoadingWidget(this);
    l->resize(width(), height());
    l->setDoneAction(a);
    setWidget(l);
    l->initialize();
}

void MainWindow::showEpisodeIntroduction(
        const stdsptr<eCampaign>& c) {
    clearWidgets();
    if(c) mCampaign = c;
    const auto e = new eEpisodeIntroductionWidget(this);
    const auto proceedA = [this]() {
        mCampaign->startEpisode();
        const auto dir = leaderSaveDir();
        saveGame(dir + "autosave replay.ez2");
        startGameAction([this]() {
            GameWidgetSettings settings;
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

std::string MainWindow::leaderSaveDir() const {
    return GameDir::saveDir() + mLeader + "/";
}

std::string MainWindow::mostRecentSavePath() const {
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

void MainWindow::clearWidgets() {
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

void MainWindow::emitWindowSizeChanged() {
    const int w = width();
    const int h = height();
    if(w <= 0 || h <= 0) return;
    if(w == mLastWindowEventW && h == mLastWindowEventH) return;
    mLastWindowEventW = w;
    mLastWindowEventH = h;
    if(!resizeCurrentWidgetToWindow()) return;
    if(mWidget == mGW && mGW) {
        mGW->updateViewBoxSize();
    } else if(mWidget == mWW && mWW) {
        mWW->update();
    }
}

void MainWindow::episodeFinished() {
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
        const auto selA = [this](const stdsptr<WorldCity>& c) {
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

void MainWindow::adventureComplete() {
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
                  Language::zeusText(62, 0),
                  mCampaign->completeText(),
                  {},
                  proceedA,
                  eEpisodeIntroType::campaingVictory);
    setWidget(e);
}

void MainWindow::episodeLost() {
    clearWidgets();
    const auto e = new eEpisodeLostWidget(this);
    const auto proceedA = [this]() {
        showMainMenu();
    };
    e->resize(width(), height());
    e->initialize(proceedA);
    setWidget(e);
}

bool MainWindow::saveGame(const std::string& path) {
    auto ez2Path = std::filesystem::path(path);
    ez2Path.replace_extension(".ez2");
    return writeGameSaveFile(ez2Path.string(), "eZeus.ez2", mGW, mCampaign);
}

bool MainWindow::loadGame(const std::string& path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if(!file) return false;
    ReadSource source(&file);
    ReadStream src(source);
    src.readFormat();
    const auto& format = src.format();
    if(format != "eZeus.ez2") {
        printf("Invalid file '%s' format '%s', expected 'eZeus.ez2'.\n",
               path.c_str(), format.c_str());
        return false;
    }
    GameWidgetSettings s;
    {
        SaveArchive settingsAr(src);
        s.serialize(settingsAr);
    }
    const auto c = std::make_shared<eCampaign>();
    {
        SaveArchive campaignAr(src);
        c->serialize(campaignAr);
    }
    c->loadStrings();
    c->loadNumbers();
    src.handlePostFuncs();
    file.close();
    if(!c->hasCurrentEpisode()) {
        c->printCurrentEpisodeDebug();
        printf("Invalid save '%s': no current campaign episode.\n",
               path.c_str());
        return false;
    }

    startGameAction(c, s);
    return true;
}

void MainWindow::closeGame() {
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

void MainWindow::showRosterOfLeaders() {
    clearWidgets();
    eMusic::playMenuMusic();
    const auto rol = new eRosterOfLeaders(this);
    rol->resize(width(), height());
    rol->initialize();
    setWidget(rol);
}

void MainWindow::showMenuLoading() {
    const auto mlw = new eMenuLoadingWidget(this);
    mlw->setDoneAction([this]() {
        if(mAfterMenuLoadingAction) {
            if(mGW) mGW->reloadUi();
            const auto action = mAfterMenuLoadingAction;
            mAfterMenuLoadingAction = nullptr;
            action();
            return;
        }
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

void MainWindow::setAfterMenuLoadingAction(const eAction& action) {
    mAfterMenuLoadingAction = action;
}

void MainWindow::showMainMenu() {
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
        fw->intialize(Language::zeusText(1, 3),
                      dir, func, closeAct);
        execDialog(fw, true, closeAct);
    };

    const auto editGameAction = [this]() {
        showChooseGameEditMenu();
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
                   optionsAction,
                   quitAction,
                   leaderAction);
}

void MainWindow::applyGraphicsSettings(const Settings& settings) {
    const bool loadNeeded = false;
    const bool reloadUiNeeded =
        settings.fUiScale != mSettings.fUiScale ||
        settings.fTopSidebarScale != mSettings.fTopSidebarScale;
    setResolution(settings.fRes);
    setDisplayMode(settings.fDisplayMode);
    mSettings = settings;
    mSettings.write();
    GameTextures::setSettings(mSettings);
    if(loadNeeded) showMenuLoading();
    else if(reloadUiNeeded && mGW) {
        addSlot([this]() {
            if(mGW) mGW->reloadUi();
        });
    }
}

void MainWindow::showOptionsMenu() {
    showOptionsMenu(0);
}

void MainWindow::showOptionsMenu(const int initialPage) {
    const auto reopenPage = [this](const int page) {
        showMainMenu();
        showOptionsMenu(page);
    };
    const auto buildPages = [this]() {
        return getOptionsPages(this);
    };
    const auto d = new eOptionsMenu(getOptionsPages(this), this, reopenPage, buildPages);
    d->initialize(initialPage);
    execDialog(d, true, [this]() { showMainMenu(); });
}

void MainWindow::showChooseGameMenu() {
    const auto gem = new eChooseGameEditMenu(this);
    gem->resize(width(), height());
    gem->initialize(false);
    setWidget(gem);
}

void MainWindow::showChooseGameEditMenu() {
    const auto gem = new eChooseGameEditMenu(this);
    gem->resize(width(), height());
    gem->initialize(true);
    setWidget(gem);
}

void MainWindow::showGame(const stdsptr<eCampaign>& c,
                           const GameWidgetSettings& settings) {
    mCampaign = c;
    const auto e = c->currentEpisode();
    if(!e) return;
    showGame(e->fBoard, settings);
}

void MainWindow::showGame(GameBoard* b,
                           const GameWidgetSettings& settings) {
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
    mGW = new GameWidget(this);
    mGW->setBoard(b);
    mGW->resize(width(), height());
    mGW->initialize();
    mGW->setSettings(settings);
    setWidget(mGW);
}

void MainWindow::showWorld() {
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

void MainWindow::execDialog(
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

int MainWindow::exec() {
    using namespace std::chrono;
    using namespace std::chrono_literals;

    showMenuLoading();

    eMouseButton button{eMouseButton::none};
    eMouseButton buttons{eMouseButton::none};

    SDL_Event e;
    eTooltip tooltip(*this);

    const bool showFPS = false;
    const double fpsClamp = kRenderFpsCap;
    const bool capRenderFps = fpsClamp > 0.;

    int c = 0;
    int fpsVal = 0;
    bool resetRenderTargets = false;
    auto nextMusicPoll = high_resolution_clock::now();
    auto nextTooltipPoll = high_resolution_clock::now();
    const duration<double, std::milli> frameDt(capRenderFps ? 1000./fpsClamp : 0.);
    auto nextFrame = high_resolution_clock::now();
    while(!mQuit) {
        const auto fpsStart = high_resolution_clock::now();

        while(SDL_PollEvent(&e)) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            mapWindowToFrame(x, y);
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
                } else if(we == SDL_WINDOWEVENT_RESIZED ||
                          we == SDL_WINDOWEVENT_SIZE_CHANGED ||
                          we == SDL_WINDOWEVENT_MAXIMIZED) {
                    resetRenderTargets = true;
                    emitWindowSizeChanged();
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

        if(capRenderFps) {
            nextFrame += duration_cast<high_resolution_clock::duration>(frameDt);
            const auto now = high_resolution_clock::now();
            if(nextFrame < now - duration_cast<high_resolution_clock::duration>(5*frameDt)) {
                nextFrame = now;
            } else {
                std::this_thread::sleep_until(nextFrame);
            }
        }

        if(mWidget == mGW && mGW) {
            mGW->updateBeforePaint();
        }

        SDL_SetRenderTarget(mSdlRenderer, nullptr);
        SDL_RenderSetViewport(mSdlRenderer, nullptr);
        SDL_RenderSetClipRect(mSdlRenderer, nullptr);
        if(mWidget != mGW) {
            SDL_SetRenderDrawColor(mSdlRenderer, 0x0, 0x0, 0x0, 0xFF);
            SDL_RenderClear(mSdlRenderer);
        }

        ePainter p(mSdlRenderer);

        const auto now = high_resolution_clock::now();
        if(now >= nextMusicPoll) {
            eMusic::incTime();
            nextMusicPoll = now + 250ms;
        }
        if(mWidget) {
            mWidget->paint(p);
            if(now >= nextTooltipPoll) {
                tooltip.update();
                nextTooltipPoll = now + 50ms;
            }
            if(!tooltip.empty()) {
                const auto& res = resolution();
                const int pp = 25*res.multiplier();
                const int wtt = tooltip.width();
                const int htt = tooltip.height();
                int mx, my;
                SDL_GetMouseState(&mx, &my);
                mapWindowToFrame(mx, my);
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
            p.drawText(0, 0, std::to_string(fpsVal), FontColor::dark);
        }

        SDL_RenderPresent(mSdlRenderer);

        std::vector<eSlot> slots;
        std::swap(slots, mSlots);
        for(const auto& s : slots) {
            s();
        }

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
