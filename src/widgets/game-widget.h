#ifndef GameWidget_H
#define GameWidget_H

constexpr int kSimHz = 60;
constexpr int kAnimFPS = 10;
constexpr int kRenderFpsCap = 0;
constexpr double kSimStepMs = 1000.0 / kSimHz;
constexpr double kAnimStepMs = 1000.0 / kAnimFPS;
constexpr double kBaseRenderMs = 1000.0 / 60.0;

#include <chrono>
#include <deque>
#include <map>
#include <memory>

#include "emainwidget.h"
#include "eframedlabel.h"

#include "engine/etile.h"
#include "fileIO/esavearchive.h"

#include "textures/eterraintextures.h"
#include "textures/ebuildingtextures.h"

#include "buildings/epatrolbuilding.h"

#include "widgets/ebuildingmode.h"
#include "widgets/buildtools/road-tool.h"
#include "widgets/game-menu.h"
#include "engine/stamps/estamptool.h"
#include "widgets/earmymenu.h"
#include "egamemainmenu.h"
#include "etopbarwidget.h"

#include "eviewmode.h"
#include "emessage.h"
#include "widgets/toasts/toast-manager.h"

#include "engine/eeventdata.h"

class WalkableObject;
class eTerrainEditMenu;
class eDomesticatedAnimal;
struct eSanctBlueprint;
class eWorldWidget;
struct eGodMessages;
struct eHeroMessages;
class eMessageBox;
class eMessageListWidget;
class GameBoard;
class eAgoraBase;
class ePatrolSourceBuilding;
class eInfoWidget;
class FramedButton;
class eFramedWidget;
class eBuildingRenderer;
class eTilePainter;

enum class eAgoraOrientation;
enum class GodType;
enum class eHeroType;
enum class GodQuestId;
enum class eWorldDirection;

using eBuildingCreator = std::function<stdsptr<eBuilding>()>;

struct eSavedMessage {
    eEventData fEd;
    eMessage fMsg;
    bool fForcePopup = false;
};

struct GameWidgetSettings {
    bool fPaused = false;
    int fSpeedId = 2;
    int fSpeed = 16;
    int fDX = 0;
    int fDY = 0;
    eTileSize fTileSize = eTileSize::s30;
    eWorldDirection fDir = eWorldDirection::N;
    std::map<int, std::pair<int, int>> fBookmarks;

    void serialize(eSaveArchive& ar) {
        ar.field("paused", fPaused);
        ar.field("speedId", fSpeedId);
        ar.field("speed", fSpeed);
        ar.field("dx", fDX);
        ar.field("dy", fDY);
        ar.field("tileSize", fTileSize);
        ar.field("direction", fDir);
        if(ar.reading()) {
            fBookmarks.clear();
            ar.countedArrayField("bookmarks", 0,
                [this](eSaveArchive& itemAr, const int) {
                    int id = 0;
                    itemAr.field("id", id);
                    auto& b = fBookmarks[id];
                    itemAr.field("x", b.first);
                    itemAr.field("y", b.second);
                });
        } else {
            auto it = fBookmarks.begin();
            ar.countedArrayField("bookmarks", static_cast<int>(fBookmarks.size()),
                [&it](eSaveArchive& itemAr, const int) {
                    int id = it->first;
                    int x = it->second.first;
                    int y = it->second.second;
                    itemAr.field("id", id);
                    itemAr.field("x", x);
                    itemAr.field("y", y);
                    ++it;
                });
        }
    }
};

class GameWidget : public eMainWidget {
public:
    GameWidget(eMainWindow* const window);
    ~GameWidget();

    void initialize();

    void rebuildGameMenu();

    void syncBannerCursor();
    // Central cursor sync for the current build mode: erase/repair/stamp get
    // their tool cursor, every other mode (road, roadblock, plain build, none)
    // falls back to the banner/default cursor. Call after any setMode change.
    void syncModeCursor();

    void screenToWorld(const int sx, const int sy,
                       int &wx, int &wy) const;
    void pixToId(const int pixX, const int pixY,
                 int& idX, int& idY) const;

    void setViewMode(const eViewMode m);
    eViewMode viewMode() const { return mViewMode; }

    void viewFraction(const double fx, const double fy);
    void viewTile(eTile* const tile);
    eTile* viewedTile() const;
    bool tileVisible(eTile* const tile) const;
    eCityId viewedCity() const;

    void showBuyCity(const eCityId cid);
    void hideBuyCity();

    void setBoard(GameBoard* const board);

    GameWidgetSettings settings() const;
    void setSettings(const GameWidgetSettings& s);

    void updateRequestButtons();
    void showTip(const ePlayerCityTarget& target,
                 const std::string& tip);
    void showQuestion(const std::string& title,
                      const std::string& q,
                      const eAction& action);
    void showQuestion(const std::string& title,
                      const std::string& q,
                      const eResourceType resource,
                      const eAction& action);

    void updateViewBoxSize();
    void updateKeyScrollSpeed(const int speed);
    void updateTopBottomAltitude();
    void updateMinMaxAltitude();
    void updateMaps(const bool totalUpdate);
    void updateMaps(const std::vector<eTile*>& tiles);
    void updateCitiesOnBoard();

    void setWorldDirection(const eWorldDirection dir);

    void centerDialog(eWidget* const d);
    void openDialog(eWidget* const d) override;

    void showMessage(eEventData& ed, const eMessage& msg,
                     const bool prepend = false,
                     const bool forcePopup = false,
                     const bool addToList = true);
    void pauseGame() { switchPause(); }
    bool gamePaused() const { return mPaused; }
    void setMessageListWidget(eMessageListWidget* const w);
    void updateBeforePaint();
protected:
    void paintEvent(ePainter& p) override;

    bool keyPressEvent(const eKeyPressEvent& e) override;
    bool keyReleaseEvent(const eKeyPressEvent& e) override;
    bool mousePressEvent(const eMouseEvent& e) override;
    bool mouseMoveEvent(const eMouseEvent& e) override;
    bool mouseReleaseEvent(const eMouseEvent& e) override;
    bool mouseWheelEvent(const eMouseWheelEvent& e) override;
private:
    std::deque<eToast> mToasts;
    std::deque<eToast> mPendingToasts;

    void renderTargetsReset() override;
    void initializeNumbers();

    void drawXY(int worldTileX, int worldTileY,
                double& drawX, double& drawY,
                const int tileSpanW, const int tileSpanH,
                const int altitude);

    void paintStampPreview(eTilePainter& tp,
                           const eTerrainTextures& trrTexs,
                           const eBuildingTextures& builTexs,
                           int worldTileX, int worldTileY,
                           ePlayerId ppid);
    void paintAppealBuildPreview(eTilePainter& tp,
                                 const eTerrainTextures& trrTexs,
                                 eBuilding* building,
                                 eBuildingRenderer* renderer,
                                 int worldTileX, int worldTileY);
    void paintBuildPreview(eTilePainter& tp,
                           ePainter& painter,
                           const eTerrainTextures& trrTexs,
                           const eBuildingTextures& builTexs,
                           ePlayerId ppid,
                           eBuildingMode mode,
                           eWorldDirection dir,
                           int boardWidth, int boardHeight,
                           bool bridgeValid,
                           const std::vector<eTile*>& bridgetTs,
                           int sMinX, int sMaxX, int sMinY, int sMaxY);

    using eRoadPreviewPath = std::map<eTile*, int>;

    void drawRoadFootprint(eTile* tile, SDL_Color color,
                           eTilePainter& tp,
                           const eTerrainTextures& trrTexs);
    static bool isRoadBandTile(eTile* tile);
    void addPatrolWalkerPreview(eTile* start, eTile* home,
                         eRoadPreviewPath& path,
                         const std::shared_ptr<WalkableObject>& walkable,
                         bool includeAgoraRoads = false,
                         const SDL_Rect* despawnRect = nullptr);
    void addPatrolBuildingRoadPreview(ePatrolBuildingBase* building,
                         eRoadPreviewPath& path,
                         eTile*& spawnTile);
    static void addPathBands(const std::vector<eTile*>& tiles,
                             eRoadPreviewPath& path);
    static eTile* firstPathRoad(const std::vector<eTile*>& tiles);
    static eTile* lastPathRoad(const std::vector<eTile*>& tiles);
    static SDL_Color roadBandColor(int freq);
    void drawRoadBandTile(eTile* tile, eTile* start,
                          const eRoadPreviewPath& path,
                          eTilePainter& tp,
                          const eTerrainTextures& trrTexs);
    void drawRoadPreview(const eRoadPreviewPath& path,
                         eTile* spawnTile,
                         eTilePainter& tp,
                         const eTerrainTextures& trrTexs);
    void drawRoadBands(const std::vector<eTile*>& roads,
                       eTilePainter& tp,
                       const eTerrainTextures& trrTexs);

    void createGameMenu();
    void showGoals();
    void showOptionsMenu();
    void showOptionsMenu(const int initialPage);
    void showGraphicsMenu();
    void showStampManager();
    void beginStampTemplateCreate();
    void cancelStampTemplateCreate();
    void updateStampTemplateSelection();
    void updateStampTemplatePanel();
    void showStampTemplateNameDialog();
    void saveStampTemplate(const std::string& name);
    std::vector<eStampElement> stampTemplateElements(
            int* buildingCount = nullptr,
            int* roadCount = nullptr) const;
    void selectHoveredBuildingMode();
    void toggleViewMode(const eViewMode m);

    void setDX(const int dx);
    void setDY(const int dy);
    void clampViewBox();

    void setBookmark(const int id);
    void viewBookmark(const int id);

    using eApply = std::function<void(eTile* const)>;
    eApply editFunc();
    bool buildMouseRelease();
    bool buildModeAt(eBuildingMode mode,
                     int hoverTX, int hoverTY,
                     int pressedTX, int pressedTY);
    bool rightClickRelease(const eMouseEvent& e);
    bool selectedPlayerBanners() const;
    int rightDragFacing() const;
    void rightDragFormationLine(int& dx, int& dy) const;

    using eTileAction = std::function<void(eTile* const)>;
    void iterateOverVisibleTiles(const eTileAction& a);
    void playVisibleAmbientSound(const int minX, const int maxX,
                                 const int minY, const int maxY);

    void setTileSize(const eTileSize size);

    using eSpecialRequirement = std::function<bool(eTile*)>;
    bool canBuildVendor(const int tx, const int ty,
                        const eResourceType resType) const;
    bool canBuildFishery(const int tx, const int ty,
                         eDiagonalOrientation& o) const;
    bool waterTileHasAccessToSea(const int tx, const int ty) const;
    bool canBuildTriremeWharf(const int tx, const int ty,
                              eDiagonalOrientation& o) const;
    bool canBuildPier(const int tx, const int ty,
                      eDiagonalOrientation& o, const eCityId cid,
                      const ePlayerId pid, const bool forestAllowed) const;

    std::vector<eTile*> agoraBuildPlaceBR(eTile* const tile,
                                          const eCityId cid,
                                          const ePlayerId pid) const;
    std::vector<eTile*> agoraBuildPlaceTL(eTile* const tile,
                                          const eCityId cid,
                                          const ePlayerId pid) const;
    std::vector<eTile*> agoraBuildPlaceBL(eTile* const tile,
                                          const eCityId cid,
                                          const ePlayerId pid) const;
    std::vector<eTile*> agoraBuildPlaceTR(eTile* const tile,
                                          const eCityId cid,
                                          const ePlayerId pid) const;
    std::vector<eTile*> agoraBuildPlaceIter(
            eTile* const tile, const bool grand,
            eAgoraOrientation& agoraOrientation,
            const eCityId cid,
            const ePlayerId pid) const;
    std::vector<eTile*> stampAgoraBuildPlace(
            const eStampBuildCommand& cmd,
            int pressedTX, int pressedTY,
            eAgoraOrientation& agoraOrientation) const;
    bool buildStampAgora(const std::vector<eTile*>& tiles,
                         eAgoraOrientation orientation,
                         eCityId cid,
                         ePlayerId ppid);

    std::vector<ePatrolWaypoint>::iterator
        findWaypoint(const int tx, const int ty);

    void handleEvent(const eEvent e, eEventData& ed);
    void handleGodQuestEvent(eEventData& ed,
                             const bool fulfilled);
    void handleGodVisitEvent(eEventData& ed);
    void handleGodInvasionEvent(eEventData& ed);
    void handleGodHelpEvent(eEventData& ed);
    void handleSanctuaryComplete(eEventData& ed);
    void handleMonsterUnleashEvent(eEventData& ed);
    void handleMonsterInvasionInitialEvent(eEventData& ed);
    void handleMonsterInvasion24Event(eEventData& ed);
    void handleMonsterInvasion12Event(eEventData& ed);
    void handleMonsterInvasion6Event(eEventData& ed);
    void handleMonsterInvasion1Event(eEventData& ed);
    void handleMonsterInvasionEvent(eEventData& ed);
    void handleMonsterSlainEvent(eEventData& ed);
    void handleHeroArrivalEvent(eEventData& ed);

    void handleMonsterInCityEvent(eEventData& ed);

    void mapDimensions(int& mdx, int& mdy) const;
    void viewBoxSize(double& fx, double& fy) const;
    void viewedFraction(double& fx, double& fy) const;
    void tileViewFraction(eTile* const tile,
                          double& xf, double& yf) const;

    void updateMinimap();

    int rotationId() const;
    int hippodromeId() const;
    void updateHippodromeIds();

    void showMessage(eEventData& ed, const eMessageType& msg,
                     const bool prepend = false);
    void showMessage(eEventData& ed, const eEventMessageType& msg,
                     const bool prepend = false);

    void updateTipPositions();
    void updateToastPositions();
    void showToast(eEventData& ed, const eMessage& msg);
    void createToastWidget(eToast& toast);

    std::vector<eTile*> roadPath() const;
    // True if a road tile can't be placed here (building, water, etc).
    // Existing road is not blocked. Shared by build loop and red preview.
    bool roadBlocked(eTile* const t, const eCityId cid,
                     const ePlayerId pid) const;
    bool columnPath(std::vector<eOrientation>& path);
    bool bridgeTiles(eTile* const t, const eTerrain terr,
                     std::vector<eTile*>& tiles,
                     bool& rotated);
    bool canBuildAvenue(eTile* const t, const eCityId cid,
                        const ePlayerId pid,
                        const bool forestAllowed) const;

    bool inErase(const int tx, const int ty);
    bool inErase(const SDL_Rect& rect);
    bool inErase(eAgoraBase* const a);
    bool inErase(eBuilding* const b);
    eBuilding* eraseBuildingAt(const int tx, const int ty) const;
    eTile* eraseParkParentTileAt(const int tx, const int ty) const;

    bool inRepair(const int tx, const int ty);

    bool inPatrolBuildingHover(eBuilding* const b);

    void setArmyMenuVisible(const bool v);

    void scheduleConnectedTerrainUpdate(eTile* const startTile);
    void updateTerrainTextures(eTile* const tile,
                               const eTerrainTextures& trrTexs,
                               const eBuildingTextures& builTexs);
    void updateTerrainTextures();

    void updateWaypointPath();
    void setPatrolBuilding(ePatrolBuildingBase* const pb);

    void setDestinationBuilding(ePatrolSourceBuilding* const sb);
    void updateDestinationPath();
    void tickDestinationPath(const int time);

    eInfoWidget* openInfoWidget(eBuilding* const b);
    eInfoWidget* openInfoWidget(const std::vector<eCharacter *> chars);
    bool hasInfoWidget() const;

    void switchPause();
    void setSpeedId(const int id);
    void showSpeedLabel();
    bool updateSmoothScrollKey(const SDL_Scancode k, const bool pressed);
    bool smoothScrollKeyPressed(const SDL_Scancode k) const;
    void updateSmoothScrollKeysPressed();
    void smoothScroll();
    void stopSmoothScroll();
    void setKeyScrollSpeed(const int speed);
    void setGameSpeed(const int speed);

    stdsptr<eTexture> getBasementTexture(
            const int viewTileX, const int viewTileY,
            eBuilding* const building,
            const eTerrainTextures& trrTexs, const eWorldDirection dir,
            const int boardWidth, const int boardHeight);

    std::vector<eTile*> selectedTiles() const;

    eMouseButton mPressedButtons = eMouseButton::none;

    bool mEditorMode = false;
    bool mEditorShowBuildings = false;
    bool mTerrainEditMode = false;

    bool mRotate = false;
    int mRotateId = 0;

    int mDebugWomanTempleIdx = -1;

    const int sSpeeds[7] = {10, 15, 20, 40, 60, 80, 100};
    const char* const sSpeedLabels[7] = {"0.5x", "0.75x", "1x", "2x", "3x", "4x", "TURBO"};
    const int sMaxSpeedId = int(std::size(sSpeeds)) - 1;

    bool mPaused = false;
    bool mLocked = false;
    int mFrame{0};
    int mAnimFrame{0};
    int mRotateFrame{0};
    double mSimAccumMs{0.0};
    double mAnimAccumMs{0.0};
    double mRotateAccumMs{0.0};
    double mLastDtMs{kBaseRenderMs};
    std::chrono::steady_clock::time_point mLastPaintTp{};
    bool mLastPaintTpValid{false};
    std::vector<int> mValiableHippodromePieces;
    int mTime{0};
    int mDestPathLastDay{-1};
    int mLastAmbientSoundTime{0};
    std::map<int, int> mAmbientSoundCooldowns;
    int mSpeedId = 2;
    int mSpeed = sSpeeds[mSpeedId];
    std::map<int, std::pair<int, int>> mBookmarks;

    int mWheel = 0;

    int mMinAltitude = 0;
    int mMaxAltitude = 0;

    int mTopMinAltitude = 0;
    int mBottomMaxAltitude = 0;

    int mDX = 0;
    int mDY = 0;

    bool mLeftPressed = false;
    bool mRightPressed = false;
    bool mRightPanning = false;
    bool mRightFormationFacing = false;
    bool mMovedSincePress = false;
    int mSmoothScrollX = 0;
    int mSmoothScrollY = 0;
    double mSmoothScrollFineX = 0.;
    double mSmoothScrollFineY = 0.;
    int mKeyScrollSpeed = 18;

    int mHoverX = -1;
    int mHoverY = -1;
    int mHoverTX = -1;
    int mHoverTY = -1;
    int mPressedX = -1;
    int mPressedY = -1;
    int mPressedTX = -1;
    int mPressedTY = -1;
    eRoadTool mRoadTool;
    eRoadTool mColumnTool;
    int mLastX = -1;
    int mLastY = -1;

    eViewMode mViewMode = eViewMode::defaultView;

    eTileSize mTileSize = eTileSize::s30;
    int mTileW = 60;
    int mTileH = 30;
    int mScale = 100;
    double mZoom = 1.0;
    std::shared_ptr<eTexture> mWorldTex;
    std::shared_ptr<eTexture> mCompassTex;
    int mCompassDir = -1;

    int mUpdateRect = 0;
    std::vector<SDL_Rect> mUpdateRects;
    stdptr<GameBoard> mBoard;

    bool mDrawElevation = true;
    stdptr<eBuilding> mWalkerBuilding;
    stdptr<ePatrolBuildingBase> mPatrolBuilding;
    stdptr<ePatrolBuildingBase> mPatrolHighlightBuilding;
    stdptr<ePatrolSourceBuilding> mDestinationBuilding;
    std::vector<eTile*> mDestinationPath;
    std::vector<eBuilding*> mDestinationTargets;
    std::vector<eTile*> mWaypointOutPath;
    std::vector<eTile*> mWaypointReturnPath;
    std::vector<eTile*> mWaypointOutPath1;
    std::vector<eTile*> mWaypointReturnPath1;
    eWidget* mWaypointPathWid = nullptr;
    std::vector<ePatrolWaypoint> mSavedWaypoints;
    eViewMode mSavedViewMode = eViewMode::defaultView;

    eFramedLabel* mPausedLabel = nullptr;
    eFramedLabel* mSpeedLabel = nullptr;
    int mSpeedLabelHideFrame = 0;
    eFramedLabel* mZoomLabel = nullptr;
    int mZoomLabelHideFrame = 0;

    eTopBarWidget* mTopBar = nullptr;
    eMessageBox* mMsgBox = nullptr;
    std::deque<eSavedMessage> mSavedMsgs;
    eMessageListWidget* mMsgListWidget = nullptr;
    eTerrainEditMenu* mTem = nullptr;
    GameMenu* mGm = nullptr;
    eWidget* mPierDebugPanel = nullptr;
    eArmyMenu* mAm = nullptr;
    stdsptr<eStampTool> mStampTool;
    bool mCreatingStampTemplate = false;
    std::vector<eTile*> mStampTemplateTiles;
    eFramedWidget* mStampTemplatePanel = nullptr;
    eLabel* mStampTemplateStats = nullptr;
    bool mStampTemplateNameDialogOpen = false;

    eWorldWidget* mWW = nullptr;

    struct eTip {
        ePlayerCityTarget fTarget;
        std::string fText;
        eWidget* fWid = nullptr;
        int fLastTick = 0;
    };

    std::deque<eTip> mTips;

    std::map<eTileSize, std::vector<stdsptr<eTexture>>> mNumbers;
    std::vector<eTile*> mInflTiles;
    std::vector<eTile*> mHoverTiles;

    eTile* mViewedTile = nullptr;
    eCityId mViewedCityId = eCityId::neutralFriendly;
    bool mUpdateViewedTileScheduled = true;

    eWidget* mBuyCityWidget = nullptr;
    eLabel* mBuyCityName = nullptr;
    eLabel* mBuyCityPrice = nullptr;
    FramedButton* mBuyCityButton = nullptr;
};

#endif // GameWidget_H
