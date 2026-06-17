#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "widgets/ewidget.h"
#include "widgets/eresolution.h"
#include "textures/terrain-textures.h"
#include "textures/god-textures.h"
#include "textures/building-textures.h"
#include "textures/character-textures.h"
#include "widgets/eoptionsmenu.h"

using eSlot = std::function<void()>;

class GameBoard;
class GameWidget;
class eWorldWidget;
class eCampaign;

struct GameWidgetSettings;

class MainWindow {
public:
    MainWindow();
    ~MainWindow();

    bool initialize(const Settings& settings);
public:

    void setWidget(eWidget* const w);
    eWidget* takeWidget();

    int exec();

    void addSlot(const eSlot& slot);

    int width() const;
    int height() const;
    const eResolution& resolution() const { return mSettings.fRes; }
    SDL_Window* window() const { return mSdlWindow; }
    SDL_Renderer* renderer() const { return mSdlRenderer; }

    void setResolution(const eResolution& res);
    void setResolution(const int resolution);
    void setDisplayMode(const DisplayMode mode);
    void setInterpolation(const int interpolation);
    void setUpscale(const int upscale);
    void setUpscaleFactor(const int factor);
    void setUiScale(const int scale);
    void setDisplayMode(const int mode);
    void applyPostprocessFilters();
    void setKeyScrollSpeed(const int speed);
    void setDisableEdgeScroll(const bool b);
    void setGameSpeed(const int speed);
    void setHotkey(const HotkeyId id, const SDL_Scancode key);
    void setGeneralVolume(const int volume);
    void setMusicVolume(const int volume);
    void setVoiceVolume(const int volume);
    void setEventVolume(const int volume);
    void setAmbientVolume(const int volume);
    void setWarehouseDefaultAcceptNone(const bool b);
    void setDoubleCartCapacity(const bool b);
    void setAgorasTakeFromTradingPosts(const bool b);
    void setEnableYearlyAutosaves(const bool b);
    void setPopupForInvasion(const bool b);
    void setPopupForRequests(const bool b);
    void setPopupForTributes(const bool b);
    void setPopupForTroops(const bool b);
    void setLastDifficulty(const Difficulty d);

    void startGameAction(GameBoard* const board,
                         const GameWidgetSettings& settings);
    void startGameAction(const stdsptr<eCampaign>& c,
                         const GameWidgetSettings& settings);
    void startGameAction(const eAction& a);
    void episodeFinished();
    void adventureComplete();
    void episodeLost();

    bool saveGame(const std::string& path);
    bool loadGame(const std::string& path);
    void closeGame();

    void showRosterOfLeaders();
    void showMenuLoading();
    void setAfterMenuLoadingAction(const eAction& action);
    void showMainMenu();
    void applyGraphicsSettings(const Settings& settings);
    void showOptionsMenu();
    void showOptionsMenu(const int initialPage);
    void showChooseGameMenu();
    void showChooseGameEditMenu();
    void showGame(const stdsptr<eCampaign>& c,
                  const GameWidgetSettings& settings);
    void showGame(GameBoard* b,
                  const GameWidgetSettings& settings);
    void showWorld();

    eWidget* currentWidget() const { return mWidget; }
    eWorldWidget* worldWidget() const { return mWW; }

    const Settings& settings() const { return mSettings; }

    void execDialog(eWidget* const d,
                    const bool closable = true,
                    const eAction& closeFunc = nullptr,
                    eWidget* const parent = nullptr);

    void showQuestion(const std::string& title,
                      const std::string& text,
                      const eAction& accept,
                      const eAction& decline = nullptr);

    void showEpisodeIntroduction(const stdsptr<eCampaign>& c = nullptr);
    const stdsptr<eCampaign>& campaign() const { return mCampaign; }

    const std::string& leader() const { return mLeader; }
    void setLeader(const std::string& leader) { mLeader = leader; }
    std::string leaderSaveDir() const;
private:
    void clearWidgets();
    std::string mostRecentSavePath() const;
    bool resizeCurrentWidgetToWindow();
    bool isWindowMaximized() const;
    void emitWindowSizeChanged();
    // Map raw window mouse coords before dispatch. The city shader now runs inside
    // GameWidget only, so global UI coordinates stay in window space.
    void mapWindowToFrame(int& x, int& y) const;

    Settings mSettings;

    std::string mLeader;

    bool mQuit = false;
    bool mFirstDisplayModeSetting = true;
    bool mFirstResolutionSetting = true;
    int mLastWindowEventW = 0;
    int mLastWindowEventH = 0;

    std::vector<eSlot> mSlots;
    eAction mAfterMenuLoadingAction;

    int mShiftPressed = 0;
    int mCtrlPressed = 0;

    stdsptr<eCampaign> mCampaign;
    GameBoard* mBoard = nullptr;
    GameWidget* mGW = nullptr;
    eWorldWidget* mWW = nullptr;

    eWidget* mWidget = nullptr;
    SDL_Window* mSdlWindow = nullptr;
    SDL_Renderer* mSdlRenderer = nullptr;
};

#endif // MAIN_WINDOW_H
