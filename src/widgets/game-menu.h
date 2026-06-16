#ifndef GAME_MENU_H
#define GAME_MENU_H

#include "game-menu-base.h"
#include "ebuildingmode.h"

class eCheckableButton;
class TextureCollection;
class InterfaceTextures;
class eButton;
class GameBoard;
class PopulationDataWidget;
class eEmploymentDataWidget;
class eAdminDataWidget;
class StorageSidebarPanel;
class eAppealDataWidget;
class eHygieneSafetyDataWidget;
class eHusbandryDataWidget;
class eMythologyDataWidget;
class eCultureDataWidget;
class eScienceDataWidget;
class eMilitaryDataWidget;
class OverviewDataWidget;
class eMiniMap;
class GameWidget;
class eEventWidget;
class eBuildButton;
class eBuildWidget;
class eFramedLabel;
enum class eEvent;
struct eEventData;
class eRotateButton;
class eDataWidget;
class eMessageListWidget;

struct eSubButtonData;

class eSubButton;

struct eSPR {
    eBuildingMode fMode;
    std::string fName;
    int fMarbleCost = 0;
    int fCity = -1;
};

class GameMenu : public GameMenuBase {
public:
    using GameMenuBase::GameMenuBase;
    ~GameMenu();
    void initialize(GameBoard* const b,
                    const eAction& goalsView);
    void rebuildForResolutionChange();

    int tradeCityId() const { return mTradeCityId; }
    eBuildingMode mode() const { return mMode; }
    void clearMode() { setMode(eBuildingMode::none); }
    void setMode(const eBuildingMode mode);

    void setGameWidget(GameWidget* const gw);

    eMiniMap* miniMap() const;

    void pushEvent(const eEvent e, const eEventData& ed);

    using eViewTileHandler = std::function<void(eTile*)>;
    void setViewTileHandler(const eViewTileHandler& h);

    void closeBuildWidget();
    void setBuildWidget(eBuildWidget* const bw);

    void updateButtonsVisibility();
    void viewedCityChanged();
    void openBuildWidget(const int cmx, const int cmy,
                         const std::vector<eSPR>& cs);

    void setModeChangedAction(const eAction& func);

    void updateRequestButtons();

    void setWorldDirection(const eWorldDirection dir);

    void update();

    void setShowAllPossibleBuildings(const bool b);
protected:
    bool mousePressEvent(const eMouseEvent& e);
private:
    using eButtonsDataVec = std::vector<eSubButtonData>;
    eWidget* createSubButtons(const int resoltuionMult,
                              const eButtonsDataVec& buttons);
    eBuildButton* createBuildButton(const eSPR& c);
    eBuildButton* createTradeBuildButton(const eSPR& c,
                                         const int tradeIconWidth);

    void displayPrice(const int price, const int loc);
    eWidget* createPriceWidget(const InterfaceTextures& coll);

    GameBoard* mBoard{nullptr};
    GameWidget* mGW = nullptr;

    eBuildWidget* mBuildWidget = nullptr;

    eFramedLabel* mNameLabel = nullptr;

    eCheckableButton* mPopulationButton = nullptr;
    eCheckableButton* mHusbandryButton = nullptr;
    eCheckableButton* mIndustryButton = nullptr;
    eCheckableButton* mDistributionButton = nullptr;
    eCheckableButton* mHygieneSafetyButton = nullptr;
    eCheckableButton* mAdministrationButton = nullptr;
    eCheckableButton* mScienceButton = nullptr;
    eCheckableButton* mCultureButton = nullptr;
    eCheckableButton* mMythologyButton = nullptr;
    eCheckableButton* mMilitaryButton = nullptr;
    eCheckableButton* mAesthethicsButton = nullptr;
    eCheckableButton* mOverviewButton = nullptr;
    eButton* mMessagesButton = nullptr;
    eMessageListWidget* mMsgListW = nullptr;
    eLabel* mMsgBadge = nullptr;

    PopulationDataWidget* mPopDataW = nullptr;
    eEmploymentDataWidget* mEmplDataW = nullptr;
    eHusbandryDataWidget* mHusbDataW = nullptr;
    StorageSidebarPanel* mStrgDataW = nullptr;
    eAppealDataWidget* mApplDataW = nullptr;
    eHygieneSafetyDataWidget* mHySaDataW = nullptr;
    eAdminDataWidget* mAdminDataW = nullptr;
    eCultureDataWidget* mCultureDataW = nullptr;
    eScienceDataWidget* mScienceDataW = nullptr;
    eMythologyDataWidget* mMythDataW = nullptr;
    eMilitaryDataWidget* mMiltDataW = nullptr;
    OverviewDataWidget* mOverDataW = nullptr;

    eRotateButton* mRotateButton = nullptr;
    eButton* mWorldButton = nullptr;
    eButton* mUndoButton = nullptr;

    eMiniMap* mMiniMap = nullptr;

    int mTradeCityId = -1;
    eBuildingMode mMode{eBuildingMode::none};

    std::vector<eWid> mWidgets;

    eEventWidget* mEventW = nullptr;

    std::vector<eWidget*> mPriceWidgets;
    std::vector<eLabel*> mPriceLabels;

    std::vector<eSubButton*> mSubButtons;

    eAction mModeChangeAct;
    eAction mGoalsView;

    bool mShowAllPossibleBuildings = false;
    bool mMsgListWasPaused = false;
};

#endif // GAME_MENU_H
