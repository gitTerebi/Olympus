#ifndef EGAMEWIDGET_H
#define EGAMEWIDGET_H

constexpr int kBaseFPS = 10;
constexpr int kFpsClamp = 60;

#include <deque>

#include "emainwidget.h"
#include "eframedlabel.h"

#include "engine/etile.h"
#include "fileIO/esavearchive.h"

#include "textures/eterraintextures.h"
#include "textures/ebuildingtextures.h"

#include "buildings/epatrolbuilding.h"

#include "widgets/ebuildingmode.h"
#include "widgets/egamemenu.h"
#include "engine/stamps/estamptool.h"
#include "widgets/earmymenu.h"
#include "egamemainmenu.h"
#include "etopbarwidget.h"

#include "eviewmode.h"
#include "emessage.h"

#include "engine/eeventdata.h"

class eTerrainEditMenu;
class eDomesticatedAnimal;
struct eSanctBlueprint;
class eWorldWidget;
struct eGodMessages;
struct eHeroMessages;
class eMessageBox;
class eMessageListWidget;
class eGameBoard;
class eAgoraBase;
class eInfoWidget;
class eFramedButton;

enum class eAgoraOrientation;
enum class eGodType;
enum class eHeroType;
enum class eGodQuestId;
enum class eWorldDirection;

using eBuildingCreator = std::function<stdsptr<eBuilding>()>;

struct eSavedMessage {
    eEventData fEd;
    eMessage fMsg;
    bool fForcePopup = false;
};

struct eGameWidgetSettings {
    bool fPaused = false;
    int fSpeedId = 2;
    int fSpeed = 16;
    int fDX = 0;
    int fDY = 0;
    eTileSize fTileSize = eTileSize::s30;
    eWorldDirection fDir = eWorldDirection::N;
    std::map<int, std::pair<int, int>> fBookmarks;

    void read(eReadStream& src) {
        eSaveArchive ar(src);
        ar.field("paused", fPaused);
        ar.field("speedId", fSpeedId);
        ar.field("speed", fSpeed);
        ar.field("dx", fDX);
        ar.field("dy", fDY);
        ar.field("tileSize", fTileSize);
        ar.field("direction", fDir);

        int n;
        ar.field("bookmarkCount", n);
        for(int i = 0; i < n; i++) {
            int id;
            ar.field("bookmarkId", id);
            auto& b = fBookmarks[id];
            ar.field("bookmarkX", b.first);
            ar.field("bookmarkY", b.second);
        }
    }

    void write(eWriteStream& dst) const {
        eSaveArchive ar(dst);
        ar.field("paused", const_cast<bool&>(fPaused));
        ar.field("speedId", const_cast<int&>(fSpeedId));
        ar.field("speed", const_cast<int&>(fSpeed));
        ar.field("dx", const_cast<int&>(fDX));
        ar.field("dy", const_cast<int&>(fDY));
        ar.field("tileSize", const_cast<eTileSize&>(fTileSize));
        ar.field("direction", const_cast<eWorldDirection&>(fDir));

        int bookmarkCount = fBookmarks.size();
        ar.field("bookmarkCount", bookmarkCount);
        for(const auto& b : fBookmarks) {
            auto id = b.first;
            auto x = b.second.first;
            auto y = b.second.second;
            ar.field("bookmarkId", id);
            ar.field("bookmarkX", x);
            ar.field("bookmarkY", y);
        }
    }
};

class eGameWidget : public eMainWidget {
public:
    eGameWidget(eMainWindow* const window);
    ~eGameWidget();

    void initialize();

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

    void setBoard(eGameBoard* const board);

    eGameWidgetSettings settings() const;
    void setSettings(const eGameWidgetSettings& s);

    void updateRequestButtons();
    void showTip(const ePlayerCityTarget& target,
                 const std::string& tip);
    void showQuestion(const std::string& title,
                      const std::string& q,
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
protected:
    void paintEvent(ePainter& p) override;

    bool keyPressEvent(const eKeyPressEvent& e) override;
    bool keyReleaseEvent(const eKeyPressEvent& e) override;
    bool mousePressEvent(const eMouseEvent& e) override;
    bool mouseMoveEvent(const eMouseEvent& e) override;
    bool mouseReleaseEvent(const eMouseEvent& e) override;
    bool mouseWheelEvent(const eMouseWheelEvent& e) override;
private:
    void renderTargetsReset() override;
    void initializeNumbers();

    void drawXY(int tx, int ty,
                double& rx, double& ry,
                const int wSpan, const int hSpan,
                const int a);

    void paintStampPreview(eTilePainter& tp,
                           const eTerrainTextures& trrTexs,
                           const eBuildingTextures& builTexs,
                           int tx, int ty,
                           ePlayerId ppid);

    void showGoals();
    void showOptionsMenu();
    void showGraphicsMenu();
    void showStampManager();
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
            eAgoraOrientation& bt,
            const eCityId cid,
            const ePlayerId pid) const;
    std::vector<eTile*> stampAgoraBuildPlace(
            const eStampBuildCommand& cmd,
            int pressedTX, int pressedTY,
            eAgoraOrientation& bt) const;
    bool buildStampAgora(const std::vector<eTile*>& tiles,
                         eAgoraOrientation orientation,
                         eCityId cid,
                         ePlayerId ppid);

    std::vector<ePatrolGuide>::iterator
        findGuide(const int tx, const int ty);

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

    bool roadPath(std::vector<eOrientation>& path);
    std::vector<eTile*> roadPath() const;
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

    void updatePatrolPath();
    void setPatrolBuilding(ePatrolBuildingBase* const pb);

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
            const int tx, const int ty, eBuilding* const d,
            const eTerrainTextures& trrTexs, const eWorldDirection dir,
            const int boardw, const int boardh);

    std::vector<eTile*> selectedTiles() const;

    eMouseButton mPressedButtons = eMouseButton::none;

    bool mEditorMode = false;
    bool mEditorShowBuildings = false;
    bool mTerrainEditMode = false;

    bool mRotate = false;
    int mRotateId = 0;

    const int sSpeeds[7] = {8, 12, 16, 32, 48, 64, 80};
    const char* const sSpeedLabels[7] = {"0.5x", "0.75x", "1x", "2x", "3x", "4x", "TURBO"};
    const int sMaxSpeedId = int(std::size(sSpeeds)) - 1;

    bool mPaused = false;
    bool mLocked = false;
    int mFrame{0};
    int mAnimFrame{0};
    int mRotateFrame{0};
    std::vector<int> mValiableHippodromePieces;
    int mTime{0};
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
    bool mScrollLeft = false;
    bool mScrollRight = false;
    bool mScrollUp = false;
    bool mScrollDown = false;
    int mSmoothScrollX = 0;
    int mSmoothScrollY = 0;
    int mKeyScrollSpeed = 18;

    int mHoverX = -1;
    int mHoverY = -1;
    int mHoverTX = -1;
    int mHoverTY = -1;
    int mPressedX = -1;
    int mPressedY = -1;
    int mPressedTX = -1;
    int mPressedTY = -1;
    int mLastX = -1;
    int mLastY = -1;

    eViewMode mViewMode = eViewMode::defaultView;

    eTileSize mTileSize = eTileSize::s30;
    int mTileW = 60;
    int mTileH = 30;
    int mScale = 100;

    int mUpdateRect = 0;
    std::vector<SDL_Rect> mUpdateRects;
    stdptr<eGameBoard> mBoard;

    bool mDrawElevation = true;
    stdptr<eBuilding> mWalkerBuilding;
    stdptr<ePatrolBuildingBase> mPatrolBuilding;
    stdptr<ePatrolBuildingBase> mPatrolHighlightBuilding;
    std::vector<eTile*> mPatrolPath;
    std::vector<eTile*> mExcessPatrolPath;
    std::vector<eTile*> mPatrolPath1;
    std::vector<eTile*> mExcessPatrolPath1;
    eWidget* mPatrolPathWid = nullptr;
    std::vector<ePatrolGuide> mSavedGuides;
    eViewMode mSavedViewMode = eViewMode::defaultView;

    eFramedLabel* mPausedLabel = nullptr;
    eFramedLabel* mSpeedLabel = nullptr;
    int mSpeedLabelHideFrame = 0;

    eTopBarWidget* mTopBar = nullptr;
    eMessageBox* mMsgBox = nullptr;
    std::deque<eSavedMessage> mSavedMsgs;
    eMessageListWidget* mMsgListWidget = nullptr;
    eTerrainEditMenu* mTem = nullptr;
    eGameMenu* mGm = nullptr;
    eArmyMenu* mAm = nullptr;
    stdsptr<eStampTool> mStampTool;

    eWorldWidget* mWW = nullptr;

    struct eTip {
        ePlayerCityTarget fTarget;
        std::string fText;
        eWidget* fWid = nullptr;
        int fLastFrame = 0;
    };

    std::deque<eTip> mTips;

    // Toast notification - temporary UI element, history already logged when showMessage was called
    struct eToast {
        eEventData fEd;
        eMessage fMsg;
        eWidget* fWid = nullptr;
        int fExpireFrame = 0;
        eDate fDate; // Date when toast was created (for potential debugging/analytics)
    };

    std::deque<eToast> mToasts;

    std::map<eTileSize, std::vector<stdsptr<eTexture>>> mNumbers;
    std::vector<eTile*> mInflTiles;
    std::vector<eTile*> mHoverTiles;

    eTile* mViewedTile = nullptr;
    eCityId mViewedCityId = eCityId::neutralFriendly;
    bool mUpdateViewedTileScheduled = true;

    eWidget* mBuyCityWidget = nullptr;
    eLabel* mBuyCityName = nullptr;
    eLabel* mBuyCityPrice = nullptr;
    eFramedButton* mBuyCityButton = nullptr;
};

#endif // EGAMEWIDGET_H
