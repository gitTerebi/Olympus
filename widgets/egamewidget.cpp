#include "egamewidget.h"
#include "engine/stamps/estamptool.h"
#include "engine/stamps/stamp-template-writer.h"
#include "ecursors.h"

#include "emodal.h"
#include "e-message-list-widget.h"
#include "eoptionsdata.h"
#include "egamedir.h"
#include "engine/e-game-board.h"
#include "engine/egifthelpers.h"
#include "engine/e-worldcity.h"
#include "characters/gods/egod.h"
#include "characters/monsters/emonster.h"
#include "characters/echaracter.h"
#include "characters/actions/ecomplexaction.h"
#include "elanguage.h"
#include "estringhelpers.h"

#include "eflatbutton.h"
#include "eframedlabel.h"
#include "elineedit.h"

#include <filesystem>
#include <algorithm>
#include <cctype>
#include <climits>
#include <cmath>
#include <set>

void formatStoredMessage(eMessage& msg,
                         const eEventData& ed,
                         const std::string& playerName) {
    auto formatText = [&](std::string& text) {
        eStringHelpers::replaceAll(text, "[greeting]",
                                   eLanguage::text("greetings"));
        eStringHelpers::replaceAll(text, "[player_name]", playerName);
        eStringHelpers::replaceAll(text, "[god]", eGod::sGodName(ed.fGod));
        eStringHelpers::replaceAll(text, "[monster]",
                                   eMonster::sMonsterName(ed.fMonster));
        eStringHelpers::replaceAll(text, "[amount]",
                                   std::to_string(ed.fResourceCount));
        eStringHelpers::replaceAll(text, "[amount_granted]",
                                   std::to_string(ed.fResourceCount));
        eStringHelpers::replaceAll(text, "[item]",
                                   eResourceTypeHelpers::typeLongName(ed.fResourceType));
        eStringHelpers::replaceAll(text, "[itemshort]",
                                   eResourceTypeHelpers::typeName(ed.fResourceType));
        if(ed.fTime > 0) {
            const auto time = std::to_string(ed.fTime);
            eStringHelpers::replaceAll(text, "[time_until_attack]", time);
            eStringHelpers::replaceAll(text, "[time_allotted]", time);
            eStringHelpers::replaceAll(text, "[travel_time]", time);
        }
        const int giftSize = eGiftHelpers::giftCount(ed.fResourceType);
        if(giftSize > 0) {
            const int size = ed.fResourceCount/giftSize;
            std::string giftSizeText;
            if(size < 2) giftSizeText = eLanguage::zeusText(162, 0);
            else if(size < 3) giftSizeText = eLanguage::zeusText(162, 1);
            else giftSizeText = eLanguage::zeusText(162, 2);
            eStringHelpers::replaceAll(text, "[gift_size]", giftSizeText);
        }
        if(const auto c = ed.fCity) {
            const auto nat = eWorldCity::sNationalityName(c->nationality());
            eStringHelpers::replaceAll(text, "[nationality]", nat);
            eStringHelpers::replaceAll(text, "[city_name]", c->name());
            eStringHelpers::replaceAll(text, "[last_colony]", c->name());
            eStringHelpers::replaceAll(text, "[leader_name]", c->leader());
            eStringHelpers::replaceAll(text, "[a_foreign_army]", c->anArmy());
        }
        const auto c = ed.fRivalCity ? ed.fRivalCity : ed.fCity;
        if(c) {
            const auto nat = eWorldCity::sNationalityName(c->nationality());
            eStringHelpers::replaceAll(text, "[rival_nationality]", nat);
            eStringHelpers::replaceAll(text, "[rival_city_name]", c->name());
        }
    };
    formatText(msg.fTitle);
    formatText(msg.fText);
}

#include "engine/eknownendpathfinder.h"
#include "eterraineditmenu.h"

#include "textures/egametextures.h"

#include "egamemenu.h"
#include "eminimap.h"

#include "characters/actions/eanimalaction.h"

#include "characters/esoldierbanner.h"

#include "audio/sounds.h"

#include "engine/boardData/eheatmaptask.h"
#include "engine/epathfinder.h"

#include "emainwindow.h"
#include "esettings.h"

#include "eframedbutton.h"
#include "eframedwidget.h"
#include "elabel.h"
#include "eoptionsmenu.h"
#include "egraphicsmenu.h"
#include "widgets/eboardsettingsmenu.h"
#include "widgets/eflatbutton.h"
#include "widgets/infowidgets/einfowidget.h"
#include "e-message-box.h"

#include "buildings/eagoraspace.h"
#include "buildings/eagorabase.h"
#include "buildings/eaestheticsbuilding.h"
#include "buildings/evendor.h"
#include "buildings/eroad.h"
#include "buildings/eruins.h"
#include "buildings/ebuildingrenderer.h"
#include "buildings/sanctuaries/esanctbuilding.h"
#include "buildings/sanctuaries/esanctuary.h"
#include "buildings/epalace.h"
#include "buildings/epalacetile.h"
#include "buildings/ehorseranch.h"
#include "buildings/ehorseranchenclosure.h"
#include "buildings/eaestheticsbuilding.h"
#include "buildings/egatehouse.h"
#include "buildings/etradepost.h"
#include "buildings/epier.h"

#include "elanguage.h"

#include "widgets/eloadgame.h"
#include "widgets/estampmanager.h"
#include "evectorhelpers.h"
#include "etilehelper.h"
#include "widgets/equestionwidget.h"

#include "widgets/eenlistforcesdialog.h"
#include "widgets/eepisodeintroductionwidget.h"
#include "widgets/eworldwidget.h"
#include "engine/ecampaign.h"
#include "audio/music.h"
#include "spawners/ebanner.h"

#include "ebuildablehelpers.h"
#include "characters/etrireme.h"

#include "edistrictconditionswidget.h"
#include "buildings/ehippodromepiece.h"
#include "gameworld/ehitdetection.h"
#include "estringhelpers.h"

#include <algorithm>
#include <cmath>

namespace
{
    int ambientCooldownKey(eTile *const tile)
    {
        if (!tile)
            return -1;
        if (tile->onFire())
            return 1;
        if (const auto b = tile->underBuilding())
        {
            return 1000 + static_cast<int>(b->type());
        }
        const auto &chars = tile->characters();
        for (const auto &c : chars)
        {
            return 2000 + static_cast<int>(c->type());
        }
        return 3000 + static_cast<int>(tile->terrain());
    }
}

eGameWidget::eGameWidget(eMainWindow *const window) : eMainWidget(window) {
    mStampTool = std::make_shared<eStampTool>();
}

eGameWidget::~eGameWidget()
{
    setBoard(nullptr);
}

void eGameWidget::setBoard(eGameBoard *const board)
{
    if (mBoard == board)
        return;
    if (mBoard)
    {
        if (mEditorShowBuildings)
        {
            mBoard->saveEditorCityPlan();
            mBoard->editorClearBuildings();
            mBoard->emptyRubbish();
        }
        mBoard->setEventHandler(nullptr);
        mBoard->setRequestUpdateHandler(nullptr);
        mBoard->setVisibilityChecker(nullptr);
        mBoard->setButtonsVisUpdater(nullptr);
        mBoard->setMessageShower(nullptr);
        mBoard->setTipShower(nullptr);
        mBoard->setEpisodeFinishedHandler(nullptr);
        mBoard->setAutosaver(nullptr);
        mBoard->setEnlistForcesRequest(nullptr);
    }
    mBoard = board;
    if (!mBoard)
        return;
    mBoard->setEventHandler([this](const eEvent e, eEventData &ed)
                            { handleEvent(e, ed); });
    mBoard->setRequestUpdateHandler([this]()
                                    { updateRequestButtons(); });
    mBoard->setVisibilityChecker([this](eTile *const tile)
                                 { return tileVisible(tile); });
    mBoard->setButtonsVisUpdater([this]()
                                 { mGm->updateButtonsVisibility(); });
    mBoard->setMessageShower([this](eEventData &ed, const eMessageType &msg)
                             { showMessage(ed, msg); });
    mBoard->setTipShower([this](const ePlayerCityTarget &target,
                                const std::string &tip)
                         { showTip(target, tip); });
    mBoard->setEpisodeFinishedHandler([this]()
                                      {
        mLocked = true;
        const auto w = window();
        const auto c = w->campaign();

        const auto e = new eEpisodeIntroductionWidget(w);
        const auto proceedA = [w]() {
            w->episodeFinished();
        };
        e->resize(width(), height());
        const auto ee = c->currentEpisode();

        const auto path = c->currentEpisodeAudioFilePath(false);
        const bool played = eMusic::playCampaignVoice(path);
        if(!played) eMusic::playMissionVictoryMusic();

        e->initialize(c,
                      eLanguage::zeusText(62, 0),
                      ee->fComplete,
                      mBoard->goals(),
                      proceedA,
                      eEpisodeIntroType::victory);
        addWidget(e);
        e->align(eAlignment::vcenter);
        e->setX(x() + (width() - e->width() - mGm->width())/2); });
    mBoard->setAutosaver([this]()
                         {
        if(!window()->settings().fEnableYearlyAutosaves) return;
        mBoard->waitUntilFinished();
        const auto w = window();
        const auto dir = w->leaderSaveDir();
        const int year = mBoard->date().year();
        std::string yearStr;
        if(year < 0) yearStr = std::to_string(-(year  + 1)) + " BC";
        else yearStr = std::to_string(year);
        const auto filename = "autosave year " + yearStr + ".ez2";
        w->saveGame(dir + filename);
        // Clean up old autosaves, keep only 5 most recent
        std::vector<std::pair<std::filesystem::file_time_type, std::filesystem::path>> autosaves;
        for(const auto& entry : std::filesystem::directory_iterator(dir)) {
            const auto path = entry.path();
            if(path.extension() != ".ez2") continue;
            const auto name = path.stem().string();
            if(name.find("autosave year ") == 0) {
                autosaves.emplace_back(entry.last_write_time(), path);
            }
        }
        std::sort(autosaves.begin(), autosaves.end(),
                  [](const auto& a, const auto& b) { return a.first > b.first; });
        for(size_t i = 5; i < autosaves.size(); ++i) {
            std::filesystem::remove(autosaves[i].second);
        } });
    using eEnlistAction = std::function<void(const eEnlistedForces &, eResourceType)>;
    mBoard->setEnlistForcesRequest([this](
                                       const eEnlistedForces &enlistable,
                                       const std::vector<eCityId> &cids,
                                       const std::vector<std::string> &cnames,
                                       const std::vector<eHeroType> &heroesAbroad,
                                       const eEnlistAction &action,
                                       const std::vector<eResourceType> &plunderResources)
                                   {
        const auto w = window();
        const auto cw = w->currentWidget();
        const auto ww = w->worldWidget();
        const auto d = new eEnlistForcesDialog(window());
        d->initialize(enlistable, cids, cnames, heroesAbroad,
                      action, plunderResources);
        if(cw == ww) {
            ww->openDialog(d);
        } else {
            openDialog(d);
        } });

    mBoard->updateMusic();
    updateViewBoxSize();
    updateTopBottomAltitude();
    updateMinMaxAltitude();
}

eGameWidgetSettings eGameWidget::settings() const
{
    eGameWidgetSettings r;
    r.fPaused = mPaused;
    r.fSpeedId = mSpeedId;
    r.fSpeed = mSpeed;
    r.fTileSize = mTileSize;
    r.fDX = mDX;
    r.fDY = mDY;
    r.fDir = mBoard ? mBoard->direction() : eWorldDirection::N;
    r.fBookmarks = mBookmarks;
    return r;
}

void eGameWidget::setSettings(const eGameWidgetSettings &s)
{
    if (mPaused != s.fPaused)
        switchPause();
    const auto speedIt = std::find(std::begin(sSpeeds), std::end(sSpeeds), s.fSpeed);
    if (speedIt != std::end(sSpeeds))
    {
        setSpeedId(std::distance(std::begin(sSpeeds), speedIt));
    }
    else
    {
        setSpeedId(s.fSpeedId);
    }
    setTileSize(s.fTileSize);
    if (mBoard)
        mBoard->setWorldDirection(s.fDir);
    setDX(s.fDX);
    setDY(s.fDY);
    mGm->setWorldDirection(s.fDir);
    if (mTem)
        mTem->setWorldDirection(s.fDir);
    mBookmarks = s.fBookmarks;
}

void eGameWidget::initializeNumbers()
{
    mNumbers.clear();
    for (const auto size : {eTileSize::s15, eTileSize::s30,
                            eTileSize::s45, eTileSize::s60})
    {
        auto &numbers = mNumbers[size];
        int fs;
        switch (size)
        {
        case eTileSize::s15:
            fs = 10;
            break;
        case eTileSize::s30:
            fs = 20;
            break;
        case eTileSize::s45:
            fs = 30;
            break;
        case eTileSize::s60:
            fs = 40;
            break;
        }
        const auto font = eFonts::defaultFont(fs);
        const auto r = window()->renderer();
        for (int i = 0; i < 10; i++)
        {
            const auto tex = std::make_shared<eTexture>();
            tex->loadText(r, std::to_string(i), eFontColor::light, *font);
            numbers.push_back(tex);
        }
    }
}

void eGameWidget::initialize()
{
    mEditorMode = mBoard->editorMode();
    mKeyScrollSpeed = window()->settings().fKeyScrollSpeed * 5;
    initializeNumbers();
    mGm = new eGameMenu(window());
    const auto viewGoals = [this]()
    {
        showGoals();
    };
    mGm->initialize(mBoard, viewGoals);
    addWidget(mGm);
    mGm->align(eAlignment::right | eAlignment::top);
    mGm->setGameWidget(this);

    mGm->setViewTileHandler([this](eTile *const tile)
                            {
        if(!tile) return;
        viewTile(tile); });

    mGm->setModeChangedAction([this]()
                              {
        setPatrolBuilding(nullptr);
        if(mGm->mode() == eBuildingMode::erase) {
            eCursors::set(eCursorType::shovel);
        } else if(mGm->mode() == eBuildingMode::repair) {
            eCursors::set(eCursorType::repairMallet);
        } else if(mGm->mode() == eBuildingMode::stamp) {
            eCursors::set(eCursorType::stamp);
        }
        else {
            eCursors::set(eCursorType::defaultCursor);
        } });

    const auto mm = mGm->miniMap();
    mm->setChangeAction([this, mm]()
                        {
        double fx;
        double fy;
        mm->viewedFraction(fx, fy);
        viewFraction(fx, fy); });

    mAm = new eArmyMenu(window());
    mAm->initialize(*mBoard);
    mAm->setGameWidget(this);
    addWidget(mAm);
    mAm->align(eAlignment::right | eAlignment::top);
    mAm->hide();

    const auto mma = mAm->miniMap();
    mma->setChangeAction([this, mma]()
                         {
        double fx;
        double fy;
        mma->viewedFraction(fx, fy);
        viewFraction(fx, fy); });

    mTopBar = new eTopBarWidget(window());
    const int gw = width() - mGm->width();
    mTopBar->setWidth(gw);
    mTopBar->initialize();
    mTopBar->setBoard(mBoard);
    mTopBar->setGameWidget(this);
    addWidget(mTopBar);
    mTopBar->align(eAlignment::top);

    mTem = new eTerrainEditMenu(window());
    mTem->initialize(this, mBoard);
    addWidget(mTem);
    mTem->align(eAlignment::right | eAlignment::top);
    mTem->hide();

    const auto mm2 = mTem->miniMap();
    mm2->setChangeAction([this, mm2]()
                         {
        double fx;
        double fy;
        mm2->viewedFraction(fx, fy);
        viewFraction(fx, fy); });

    const int p = padding();

    if (mEditorMode)
    {
        const auto str = eLanguage::text("settings");
        const auto settingsButt = new eFramedButton(str, window());
        settingsButt->fitContent();
        addWidget(settingsButt);
        settingsButt->move(mGm->x() - settingsButt->width() - p,
                           mTopBar->height() + p);
        settingsButt->hide();
        settingsButt->setUnderline(false);
        settingsButt->setRenderBg(true);
        settingsButt->setPressAction([this]()
                                     {
            const auto settingsMenu = new eBoardSettingsMenu(window());
            settingsMenu->resize(width()/2, 2*height()/3);
            settingsMenu->initialize(this, *mBoard);

            window()->execDialog(settingsMenu);
            settingsMenu->align(eAlignment::center); });

        {
            const auto editorSwitch = new eFramedButton(window());
            editorSwitch->setRenderBg(true);
            editorSwitch->setUnderline(false);
            editorSwitch->setText(eLanguage::text("editor"));
            editorSwitch->fitContent();
            editorSwitch->move(mGm->x() - editorSwitch->width() - p,
                               mTopBar->height() + p);
            settingsButt->move(mGm->x() - settingsButt->width() - p,
                               editorSwitch->y() + editorSwitch->height() + p);
            editorSwitch->setPressAction([this, settingsButt]()
                                         {
                mTerrainEditMode = !mTerrainEditMode;
                mTem->setVisible(mTerrainEditMode);
                mGm->setVisible(!mTerrainEditMode);
                settingsButt->setVisible(mTerrainEditMode); });
            addWidget(editorSwitch);
            editorSwitch->setVisible(mEditorMode);
        }

        const auto cityEditorWidget = new eWidget(window());

        const auto condButton = new eFramedButton(window());
        condButton->setRenderBg(true);
        condButton->setUnderline(false);
        condButton->setText(eLanguage::text("conditions"));
        condButton->fitContent();
        cityEditorWidget->addWidget(condButton);
        condButton->setPressAction([this]()
                                   {
            const auto condsMenu = new eDistrictConditionsWidget(window());
            condsMenu->resize(width()/2, 2*height()/3);

            const auto get = [this]() {
                const auto c = mBoard->boardCityWithId(mViewedCityId);
                if(!c) return std::vector<eDistrictReadyCondition>{};
                return c->getDistrictReadyConditions();
            };

            const auto add = [this](const eDistrictReadyCondition& cond) {
                const auto c = mBoard->boardCityWithId(mViewedCityId);
                if(!c) return;
                c->addDistrictReadyCondition(cond);
            };

            const auto remove = [this](const int id) {
                const auto c = mBoard->boardCityWithId(mViewedCityId);
                if(!c) return;
                c->removeDistrictReadyCondition(id);
            };

            const auto set = [this](const int id, const eDistrictReadyCondition& cond) {
                const auto c = mBoard->boardCityWithId(mViewedCityId);
                if(!c) return;
                c->setDistrictReadyCondition(id, cond);
            };

            condsMenu->initialize(get, add, set, remove);

            window()->execDialog(condsMenu);
            condsMenu->align(eAlignment::center); });

        const auto saveButton = new eFramedButton(window());
        saveButton->setRenderBg(true);
        saveButton->setUnderline(false);
        saveButton->setText(eLanguage::zeusText(44, 74));
        saveButton->fitContent();
        cityEditorWidget->addWidget(saveButton);
        saveButton->setPressAction([this]()
                                   { mBoard->saveEditorCityPlan(); });

        const auto restoreButton = new eFramedButton(window());
        restoreButton->setRenderBg(true);
        restoreButton->setUnderline(false);
        restoreButton->setText(eLanguage::text("restore"));
        restoreButton->fitContent();
        cityEditorWidget->addWidget(restoreButton);
        restoreButton->setPressAction([this]()
                                      {
            mBoard->editorClearBuildings();
            mBoard->editorDisplayBuildings(); });

        cityEditorWidget->stackVertically(p);
        addWidget(cityEditorWidget);

        const auto cityEditorSwitch = new eFramedButton(window());
        cityEditorSwitch->setRenderBg(true);
        cityEditorSwitch->setUnderline(false);
        cityEditorSwitch->setText(eLanguage::text("city_editor"));
        cityEditorSwitch->fitContent();
        cityEditorSwitch->move(p, mTopBar->height() + p);
        cityEditorSwitch->setPressAction([this, cityEditorWidget]()
                                         {
            mEditorShowBuildings = !mEditorShowBuildings;
            mGm->setShowAllPossibleBuildings(mEditorShowBuildings);
            if(mEditorShowBuildings) {
                if(mBoard->currentDistrictId() == -1) {
                    mBoard->setCurrentDistrictId(0);
                }
                mBoard->editorDisplayBuildings();
            } else {
                mBoard->saveEditorCityPlan();
                mBoard->editorClearBuildings();
            }
            cityEditorWidget->setVisible(mEditorShowBuildings); });
        addWidget(cityEditorSwitch);
        cityEditorSwitch->setVisible(mEditorMode);
        const int y = cityEditorSwitch->y() + cityEditorSwitch->height() + p;
        cityEditorWidget->move(p, y);

        {
            int x = 0;
            const int y0 = restoreButton->y() + restoreButton->height() + p;
            int y = y0;
            const int iMax = 21;
            std::vector<eFramedButton *> iButtons;
            for (int i = 0; i < iMax; i++)
            {
                const auto iButton = new eFramedButton(window());
                iButton->setUnderline(false);
                iButton->setRenderBg(true);
                iButton->setText(std::to_string(i));
                iButton->fitContent();
                cityEditorWidget->addWidget(iButton);
                iButtons.push_back(iButton);
                iButton->move(x, y);
                const int h = iButton->height();
                y += h + p;
                const int nextBottom = y + h;
                int gx = 0;
                int gy = nextBottom;
                cityEditorWidget->mapToParent(gx, gy);
                if (gy > height() - h - 2 * p)
                {
                    y = y0;
                    x += iButton->width() + p;
                }
            }
            for (int i = 0; i < iMax; i++)
            {
                const auto iButton = iButtons[i];
                iButton->setPressAction([this, i, iMax, iButton, iButtons]()
                                        {
                    iButton->setText("*" + std::to_string(i) + "*");
                    mBoard->setCurrentDistrictId(i);
                    for(int j = 0; j < iMax; j++) {
                        if(j == i) continue;
                        const auto jButton = iButtons[j];
                        jButton->setText(std::to_string(j));
                    } });
                if (i == 0)
                    iButton->trigger();
            }
        }
        cityEditorWidget->fitContent();
        cityEditorWidget->hide();
    }

    {
        const auto buyCityWidget = new eFramedWidget(window());
        mBuyCityWidget = buyCityWidget;
        buyCityWidget->setType(eFrameType::message);
        const auto innerWidget = new eWidget(window());
        buyCityWidget->addWidget(innerWidget);
        innerWidget->move(p, p);

        const auto cityLabel = new eLabel(window());
        mBuyCityName = cityLabel;
        cityLabel->setText("City Placeholder");
        cityLabel->fitContent();
        innerWidget->addWidget(cityLabel);

        const auto priceWidget = new eWidget(window());

        const auto iconLabel = new eLabel(window());
        iconLabel->setNoPadding();
        const auto &intrfc = eGameTextures::interface();
        const auto uiScale = resolution().uiScale();
        const int icoll = static_cast<int>(uiScale);
        const auto &coll = intrfc[icoll];
        iconLabel->setTexture(coll.fDrachmasTopMenu);
        iconLabel->fitContent();
        priceWidget->addWidget(iconLabel);

        const auto priceLabel = new eLabel(window());
        mBuyCityPrice = priceLabel;
        priceLabel->setNoPadding();
        priceLabel->setText("99999");
        priceLabel->fitContent();
        priceWidget->addWidget(priceLabel);

        priceWidget->stackHorizontally(p);
        priceWidget->fitContent();
        innerWidget->addWidget(priceWidget);

        const auto button = new eFramedButton(window());
        mBuyCityButton = button;
        button->setUnderline(false);
        button->setText(eLanguage::zeusText(44, 5));
        button->fitContent();
        innerWidget->addWidget(button);

        innerWidget->stackVertically(p);
        innerWidget->fitContent();
        cityLabel->align(eAlignment::hcenter);
        priceWidget->align(eAlignment::hcenter);
        button->align(eAlignment::hcenter);
        buyCityWidget->resize(innerWidget->width() + 2 * p,
                              innerWidget->height() + 2 * p);
        addWidget(buyCityWidget);
        centerDialog(buyCityWidget);
        buyCityWidget->hide();
    }

    const auto &setts = window()->settings();
    const auto sizes = setts.availableSizes();
    setTileSize(sizes.front());
}

void eGameWidget::pixToId(const int pixX, const int pixY,
                          int &idX, int &idY) const
{
    ::pixToId(pixX, pixY, idX, idY, mScale, mTileW, mTileH, mDX, mDY, mMaxAltitude, mMinAltitude, mBoard);
}

void eGameWidget::setViewMode(const eViewMode m)
{
    mViewMode = m;
}

void eGameWidget::toggleViewMode(const eViewMode m)
{
    setViewMode(mViewMode == m ? eViewMode::defaultView : m);
}

void eGameWidget::mapDimensions(int &mdx, int &mdy) const
{
    const int w = mBoard->rotatedWidth();
    const int h = mBoard->rotatedHeight();
    mdx = mTileW * w;
    mdy = mTileH * h / 2;
}

void eGameWidget::viewBoxSize(double &fx, double &fy) const
{
    int mdx;
    int mdy;
    mapDimensions(mdx, mdy);
    fx = (width() - mGm->width()) / double(mdx);
    fy = height() / double(mdy);
}

void eGameWidget::viewedFraction(double &fx, double &fy) const
{
    int mdx;
    int mdy;
    mapDimensions(mdx, mdy);
    const int w = width() - mGm->width();
    fx = (0.5 * w - mDX) / mdx;
    fy = (0.5 * height() - mDY) / mdy;
}

void eGameWidget::tileViewFraction(eTile *const tile,
                                   double &xf, double &yf) const
{
    int mdx;
    int mdy;
    mapDimensions(mdx, mdy);
    const double tx = tile->dx() * mTileW;
    const double ty = 0.5 * tile->dy() * mTileH;
    xf = tx / mdx;
    yf = ty / mdy;
}

void eGameWidget::viewFraction(const double fx, const double fy)
{
    int mdx;
    int mdy;
    mapDimensions(mdx, mdy);

    const int w = width() - mGm->width();
    const int dx = -fx * mdx + w / 2;
    const int dy = -fy * mdy + height() / 2;
    setDX(dx);
    setDY(dy);
}

void eGameWidget::viewTile(eTile *const tile)
{
    if (!tile)
        return;
    int mdx;
    int mdy;
    mapDimensions(mdx, mdy);
    const int dtx = tile->dx();
    const int dty = tile->dy();
    const auto dir = mBoard->direction();
    const int width = mBoard->width();
    const int height = mBoard->height();
    int rdtx;
    int rdty;
    eTileHelper::dTileIdToRotatedDTileId(dtx, dty, rdtx, rdty,
                                         dir, width, height);
    const int tx = rdtx * mTileW;
    const int ty = rdty * mTileH / 2;
    const double x = double(tx) / mdx;
    const double y = double(ty) / mdy;
    viewFraction(x, y);
}

eTile *eGameWidget::viewedTile() const
{
    double fx;
    double fy;
    viewedFraction(fx, fy);
    int mdx;
    int mdy;
    mapDimensions(mdx, mdy);
    const int vx = fx * mdx / mTileW;
    const int vy = fy * mdy * 2 / mTileH;
    const auto tile = mBoard->rotateddtile(vx, vy);
    return tile;
}

bool eGameWidget::tileVisible(eTile *const tile) const
{
    if (!tile)
        return false;

    double fx;
    double fy;
    viewBoxSize(fx, fy);

    double ffx;
    double ffy;
    viewedFraction(ffx, ffy);

    double txf;
    double tyf;
    tileViewFraction(tile, txf, tyf);

    const double top = ffy - fy / 2;
    const double left = ffx - fx / 2;
    const double bottom = top + fy;
    const double right = left + fx;

    if (txf > right)
        return false;
    if (txf < left)
        return false;
    if (tyf > bottom)
        return false;
    if (tyf < top)
        return false;
    return true;
}

eCityId eGameWidget::viewedCity() const
{
    return mViewedCityId;
}

void eGameWidget::showBuyCity(const eCityId cid)
{
    const auto c = mBoard->boardCityWithId(cid);
    const int price = c->basePrice();
    mBuyCityName->setText(mBoard->cityName(cid));
    mBuyCityName->align(eAlignment::hcenter);
    mBuyCityPrice->setText(std::to_string(price));
    mBuyCityWidget->show();
    const auto ppid = mBoard->personPlayer();
    mBuyCityButton->setPressAction([this, cid, price, ppid]()
                                   {
        const int d = mBoard->drachmas(ppid);
        if(d >= price) {
            const auto& wboard = mBoard->world();
            const auto wc = wboard.cityWithId(cid);
            wc->setState(eCityState::active);
            mBoard->moveCityToPlayer(cid, ppid);
            mBoard->incDrachmas(ppid, -price, eFinanceTarget::bribesTributePaid);
            hideBuyCity();
            mGm->viewedCityChanged();
        } else {
            showTip(ppid, eLanguage::zeusText(19, 19));
        } });
}

void eGameWidget::hideBuyCity()
{
    mBuyCityWidget->hide();
}

void eGameWidget::iterateOverVisibleTiles(const eTileAction &a)
{
    const int rw = mBoard->rotatedWidth();
    const int rh = mBoard->rotatedHeight();

    const int minX = std::clamp(-mDX / mTileW, 0, rw);
    const int visWidth = width() - mGm->width();
    const int maxX = std::clamp(minX + visWidth / mTileW, 0, rw);

    const int minY = std::clamp(-2 * mDY / mTileH, 0, rh);
    const int maxY = std::clamp(minY + 2 * height() / mTileH, 0, rh);

    playVisibleAmbientSound(minX, maxX, minY, maxY);

    const int eminX = std::clamp(minX - 5, 0, rw);
    const int emaxX = std::clamp(maxX + 10, 0, rw);

    const int eminY = std::clamp(minY - 10, 0, rh);
    const int emaxY = std::clamp(maxY + 35, 0, rh);

    for (int y = eminY; y < emaxY; y++)
    {
        for (int x = eminX; x < emaxX; x++)
        {
            const auto t = mBoard->rotateddtile(x, y);
            if (!t)
                continue;
            a(t);
        }
    }
}

void eGameWidget::playVisibleAmbientSound(const int minX, const int maxX,
                                          const int minY, const int maxY)
{
    const int now = SDL_GetTicks();
    const int interval = 5000;
    const int cooldown = 60000;
    if (now - mLastAmbientSoundTime < interval)
        return;
    if (eRand::rand() % 3 != 0)
        return;

    std::vector<eTile *> buildingTiles;
    std::vector<eTile *> fallbackTiles;
    for (int y = minY; y < maxY; y++)
    {
        for (int x = minX; x < maxX; x++)
        {
            const auto t = mBoard->rotateddtile(x, y);
            if (!t)
                continue;
            if (t->underBuilding())
            {
                buildingTiles.push_back(t);
            }
            else
            {
                fallbackTiles.push_back(t);
            }
        }
    }

    const auto playFromTiles = [&](const std::vector<eTile *> &tiles)
    {
        if (tiles.empty())
            return false;
        const int size = tiles.size();
        const int startId = eRand::rand() % size;
        for (int i = 0; i < size; i++)
        {
            const int id = (startId + i) % size;
            const auto tile = tiles[id];
            const int key = ambientCooldownKey(tile);
            const auto it = mAmbientSoundCooldowns.find(key);
            if (it != mAmbientSoundCooldowns.end() &&
                now - it->second < cooldown)
            {
                continue;
            }
            eSounds::playSoundForTile(tile);
            mAmbientSoundCooldowns[key] = now;
            mLastAmbientSoundTime = now;
            return true;
        }
        return false;
    };

    if (playFromTiles(buildingTiles))
        return;
    playFromTiles(fallbackTiles);
}

bool eGameWidget::canBuildVendor(const int tx, const int ty,
                                 const eResourceType resType) const
{
    const auto t = mBoard->tile(tx, ty);
    if (!t)
        return false;
    const auto b = t->underBuilding();
    if (!b)
        return false;
    const auto bt = b->type();
    if (bt != eBuildingType::agoraSpace)
        return false;
    const auto space = static_cast<eAgoraSpace *>(b);
    const auto agora = space->agora();
    if (agora->vendor(resType))
        return false;
    const auto ct = b->centerTile();
    if (!ct)
        return false;
    return ct->x() == tx && ct->y() == ty;
}

bool tileBuildable(eTile *const t)
{
    if (!t)
        return false;
    if (t->underBuilding())
        return false;
    const auto &banners = t->banners();
    for (const auto &b : banners)
    {
        if (!b->buildable())
            return false;
    }
    if (t->isElevationTile())
        return false;
    const auto &chars = t->characters();
    if (!chars.empty())
        return false;
    return true;
}

bool eGameWidget::waterTileHasAccessToSea(const int tx, const int ty) const
{
    const auto t = mBoard->tile(tx, ty);
    if (!t)
        return false;
    if (!t->hasWater())
        return false;
    const auto cid = mViewedCityId;
    const auto riverEntry = mBoard->riverEntryPoint(cid);
    if (!riverEntry)
        return false;
    eKnownEndPathFinder p([](eTileBase *const tile)
                          { return tile->hasWater(); }, riverEntry);
    const int w = mBoard->width();
    const int h = mBoard->height();
    const bool r = p.findPath({0, 0, w, h}, t, 1000, true, w, h);
    return r;
}

bool eGameWidget::canBuildFishery(const int tx, const int ty,
                                  eDiagonalOrientation &o) const
{
    for (int x = tx; x < tx + 2; x++)
    {
        for (int y = ty - 1; y < ty - 1 + 2; y++)
        {
            const auto t = mBoard->tile(x, y);
            const bool b = tileBuildable(t);
            if (!b)
                return false;
        }
    }
    const auto t = mBoard->tile(tx, ty);
    if (!t)
        return false;
    const bool tr = eBuildableHelpers::canBuildFisheryTR(t);
    if (tr)
    {
        o = eDiagonalOrientation::topRight;
        return true;
    }
    const bool br = eBuildableHelpers::canBuildFisheryBR(t);
    if (br)
    {
        o = eDiagonalOrientation::bottomRight;
        return true;
    }
    const bool bl = eBuildableHelpers::canBuildFisheryBL(t);
    if (bl)
    {
        o = eDiagonalOrientation::bottomLeft;
        return true;
    }
    const bool tl = eBuildableHelpers::canBuildFisheryTL(t);
    if (tl)
    {
        o = eDiagonalOrientation::topLeft;
        return true;
    }
    return false;
}

bool eGameWidget::canBuildTriremeWharf(const int tx, const int ty,
                                       eDiagonalOrientation &o) const
{
    for (int x = tx - 1; x < tx - 1 + 3; x++)
    {
        for (int y = ty - 1; y < ty - 1 + 3; y++)
        {
            const auto t = mBoard->tile(x, y);
            const bool b = tileBuildable(t);
            if (!b)
                return false;
        }
    }
    {
        const auto t = mBoard->tile(tx - 1, ty);
        if (!t)
            return false;
        const bool tr = eBuildableHelpers::canBuildFisheryTR(t);
        if (tr)
        {
            const auto br = t->bottomRight<eTile>();
            const bool tr = eBuildableHelpers::canBuildFisheryTR(br);
            if (tr)
            {
                o = eDiagonalOrientation::topRight;
                return true;
            }
        }
    }
    {
        const auto t = mBoard->tile(tx, ty);
        if (!t)
            return false;
        const bool br = eBuildableHelpers::canBuildFisheryBR(t);
        if (br)
        {
            const auto bl = t->bottomLeft<eTile>();
            const bool br = eBuildableHelpers::canBuildFisheryBR(bl);
            if (br)
            {
                o = eDiagonalOrientation::bottomRight;
                return true;
            }
        }
    }
    {
        const auto t = mBoard->tile(tx - 1, ty + 1);
        if (!t)
            return false;
        const bool bl = eBuildableHelpers::canBuildFisheryBL(t);
        if (bl)
        {
            const auto br = t->bottomRight<eTile>();
            const bool bl = eBuildableHelpers::canBuildFisheryBL(br);
            if (bl)
            {
                o = eDiagonalOrientation::bottomLeft;
                return true;
            }
        }
    }
    {
        const auto t = mBoard->tile(tx - 1, ty + 1);
        if (!t)
            return false;
        const bool tl = eBuildableHelpers::canBuildFisheryTL(t);
        if (tl)
        {
            const auto tr = t->topRight<eTile>();
            const bool tl = eBuildableHelpers::canBuildFisheryTL(tr);
            if (tl)
            {
                o = eDiagonalOrientation::topLeft;
                return true;
            }
        }
    }
    return false;
}

bool eGameWidget::canBuildPier(const int tx, const int ty,
                               eDiagonalOrientation &o,
                               const eCityId cid,
                               const ePlayerId pid,
                               const bool forestAllowed) const
{
    const bool r = canBuildFishery(tx, ty, o);
    if (!r)
        return false;
    int minX;
    int minY;
    switch (o)
    {
    case eDiagonalOrientation::topRight:
    {
        minX = tx - 1;
        minY = ty + 1;
    }
    break;
    case eDiagonalOrientation::bottomRight:
    {
        minX = tx - 4;
        minY = ty - 2;
    }
    break;
    case eDiagonalOrientation::bottomLeft:
    {
        minX = tx - 1;
        minY = ty - 5;
    }
    break;
    default:
    case eDiagonalOrientation::topLeft:
    {
        minX = tx + 2;
        minY = ty - 2;
    }
    break;
    }
    return mBoard->canBuildBase(minX, minX + 4, minY, minY + 4,
                                forestAllowed, cid, pid);
}

std::vector<ePatrolGuide>::iterator
eGameWidget::findGuide(const int tx, const int ty)
{
    auto &pgs = mPatrolBuilding->patrolGuides();
    const int iMax = pgs.size();
    for (int i = 0; i < iMax; i++)
    {
        auto &pg = pgs[i];
        if (pg.fX == tx && pg.fY == ty)
        {
            return pgs.begin() + i;
        }
    }
    return pgs.end();
}

void eGameWidget::updateMinimap()
{
    double fx;
    double fy;
    viewedFraction(fx, fy);
    const auto mm = mGm->miniMap();
    mm->viewFraction(fx, fy);
    const auto mma = mAm->miniMap();
    mma->viewFraction(fx, fy);
    const auto mmt = mTem->miniMap();
    mmt->viewFraction(fx, fy);
}

const int gRotateFrames = 150;

int eGameWidget::rotationId() const
{
    return mRotateFrame / gRotateFrames;
}

int eGameWidget::hippodromeId() const
{
    if (mValiableHippodromePieces.empty())
        return -1;
    const int id = rotationId() % mValiableHippodromePieces.size();
    return mValiableHippodromePieces[id];
}

void eGameWidget::updateHippodromeIds()
{
    mValiableHippodromePieces.clear();

    const auto hs = mBoard->buildings(mViewedCityId, eBuildingType::hippodromePiece);
    if (hs.empty())
    {
        mValiableHippodromePieces = {0, 1, 2, 3, 4, 5, 6, 7};
        return;
    }

    int minX;
    int minY;
    int maxX;
    int maxY;
    eGameBoard::sBuildTiles(minX, minY, maxX, maxY,
                            mHoverTX, mHoverTY, 4, 4);
    maxY--;
    maxX--;
    bool topLeft = false;
    bool topRight = false;
    bool bottomRight = false;
    bool bottomLeft = false;

    bool topLeftBlocked = false;
    bool topRightBlocked = false;
    bool bottomRightBlocked = false;
    bool bottomLeftBlocked = false;

    const auto hippodromeAt = [&](const int x, const int y)
    {
        const auto b = mBoard->buildingAt(x, y);
        if (!b)
            return static_cast<eHippodromePiece *>(nullptr);
        const auto type = b->type();
        if (type == eBuildingType::hippodromePiece)
        {
            const auto h = static_cast<eHippodromePiece *>(b);
            return h;
        }
        else if (type == eBuildingType::road)
        {
            const auto r = static_cast<eRoad *>(b);
            return r->aboveHippodrome();
        }
        return static_cast<eHippodromePiece *>(nullptr);
    };

    {
        const int x = minX - 1;
        const auto b1 = hippodromeAt(x, minY);
        const auto b2 = hippodromeAt(x, maxY);
        if (b1 == b2 && b1 && b2)
        {
            const int id = b1->id();
            topLeftBlocked = true;
            topLeft = id == 0 || id == 4 || id == 5 || id == 7;
        }
    }
    {
        const int y = minY - 1;
        const auto b1 = hippodromeAt(minX, y);
        const auto b2 = hippodromeAt(maxX, y);
        if (b1 == b2 && b1 && b2)
        {
            const int id = b1->id();
            topRightBlocked = true;
            topRight = id == 1 || id == 2 || id == 6 || id == 7;
        }
    }
    {
        const int x = maxX + 1;
        const auto b1 = hippodromeAt(x, minY);
        const auto b2 = hippodromeAt(x, maxY);
        if (b1 == b2 && b1 && b2)
        {
            const int id = b1->id();
            bottomRightBlocked = true;
            bottomRight = id == 0 || id == 1 || id == 3 || id == 4;
        }
    }
    {
        const int y = maxY + 1;
        const auto b1 = hippodromeAt(minX, y);
        const auto b2 = hippodromeAt(maxX, y);
        if (b1 == b2 && b1 && b2)
        {
            const int id = b1->id();
            bottomLeftBlocked = true;
            bottomLeft = id == 2 || id == 3 || id == 5 || id == 6;
        }
    }

    if (topLeft && bottomRight)
    {
        mValiableHippodromePieces = {0, 4};
    }
    else if (topLeft && bottomLeft)
    {
        mValiableHippodromePieces = {1};
    }
    else if (topRight && bottomLeft)
    {
        mValiableHippodromePieces = {2, 6};
    }
    else if (topLeft && topRight)
    {
        mValiableHippodromePieces = {3};
    }
    else if (topRight && bottomRight)
    {
        mValiableHippodromePieces = {5};
    }
    else if (bottomLeft && bottomRight)
    {
        mValiableHippodromePieces = {7};
    }
    else if (topLeft)
    {
        mValiableHippodromePieces = {0, 1, 3, 4};
    }
    else if (topRight)
    {
        mValiableHippodromePieces = {2, 3, 5, 6};
    }
    else if (bottomRight)
    {
        mValiableHippodromePieces = {0, 4, 5, 7};
    }
    else if (bottomLeft)
    {
        mValiableHippodromePieces = {1, 2, 6, 7};
    }

    if (!topLeft && topLeftBlocked)
    {
        eVectorHelpers::remove(mValiableHippodromePieces, 0);
        eVectorHelpers::remove(mValiableHippodromePieces, 1);
        eVectorHelpers::remove(mValiableHippodromePieces, 3);
        eVectorHelpers::remove(mValiableHippodromePieces, 4);
    }
    if (!topRight && topRightBlocked)
    {
        eVectorHelpers::remove(mValiableHippodromePieces, 2);
        eVectorHelpers::remove(mValiableHippodromePieces, 3);
        eVectorHelpers::remove(mValiableHippodromePieces, 5);
        eVectorHelpers::remove(mValiableHippodromePieces, 6);
    }
    if (!bottomRight && bottomRightBlocked)
    {
        eVectorHelpers::remove(mValiableHippodromePieces, 0);
        eVectorHelpers::remove(mValiableHippodromePieces, 4);
        eVectorHelpers::remove(mValiableHippodromePieces, 5);
        eVectorHelpers::remove(mValiableHippodromePieces, 7);
    }
    if (!bottomLeft && bottomLeftBlocked)
    {
        eVectorHelpers::remove(mValiableHippodromePieces, 1);
        eVectorHelpers::remove(mValiableHippodromePieces, 2);
        eVectorHelpers::remove(mValiableHippodromePieces, 6);
        eVectorHelpers::remove(mValiableHippodromePieces, 7);
    }
}

void eGameWidget::showTip(const ePlayerCityTarget &target,
                          const std::string &tip)
{
    for (const auto &t : mTips)
    {
        if (t.fText == tip && t.fTarget == target)
            return;
    }
    std::string text;
    const auto ppid = mBoard->personPlayer();
    if (target.isCityTarget())
    {
        const auto cid = target.cityTarget();
        const auto pid = mBoard->cityIdToPlayerId(cid);
        if (pid != ppid)
            return;
        const auto cts = mBoard->personPlayerCitiesOnBoard();
        if (cts.size() > 1)
        {
            const auto name = mBoard->cityName(cid);
            text = name + ": " + tip;
        }
        else
        {
            text = tip;
        }
    }
    else
    {
        if (target.isPlayerTarget())
        {
            const auto pid = target.playerTarget();
            if (pid != ppid)
                return;
        }
        text = tip;
    }
    const auto msgb = new eFlatButton(window());
    msgb->setNoPadding();
    msgb->setFontSizeXS();
    msgb->setText(text);
    msgb->fitContent();
    const int p = msgb->padding();
    addWidget(msgb);
    msgb->resize(msgb->width() + 2 * p, msgb->height() + 2 * p);
    msgb->setX((width() - mGm->width() - msgb->width()) / 2);
    eTip &etip = mTips.emplace_back();
    etip.fTarget = target;
    etip.fText = tip;
    etip.fWid = msgb;
    etip.fLastFrame = mFrame + 200;
    const auto etipPtr = &etip;
    msgb->setPressAction([etipPtr]()
                         { etipPtr->fLastFrame -= 200; });
    updateTipPositions();
}

void eGameWidget::showQuestion(
    const std::string &title,
    const std::string &q,
    const eAction &action)
{

    const auto cancelA = [this]()
    {
        mLocked = false;
    };

    const auto acceptA = [this, action]()
    {
        action();
        mLocked = false;
    };

    const auto qw = new eQuestionWidget(window());
    qw->initialize(title, q, acceptA, cancelA);
    addWidget(qw);
    qw->align(eAlignment::vcenter);
    const int vw = width() - mGm->width();
    const int w = qw->width();
    qw->setX((vw - w) / 2);
    mLocked = true;
}

void eGameWidget::updateTipPositions()
{
    const int p = padding();
    int y;
    if (mPausedLabel)
    {
        y = mPausedLabel->y() + mPausedLabel->height() + 2 * p;
    }
    else
    {
        y = 5 * p;
        ;
    }
    if (mSpeedLabel && mSpeedLabel->visible())
    {
        mSpeedLabel->setY(y);
        y += mSpeedLabel->height() + 2 * p;
    }
    for (const auto &tip : mTips)
    {
        const auto w = tip.fWid;
        w->setY(y);
        const int wh = w->height();
        y += wh + 2 * p;
    }
}

bool eGameWidget::roadPath(std::vector<eOrientation> &path)
{
    const auto allowed = mEditorMode ? eTerrain::buildableAfterClear : eTerrain::buildable;
    ePathFinder p([allowed](eTileBase *const t)
                  {
        const auto terr = t->terrain();
        const bool tr = static_cast<bool>(allowed & terr);
        if(!tr) return false;
        const auto bt = t->underBuildingType();
        const bool r = bt == eBuildingType::road ||
                       bt == eBuildingType::none;
        if(!r) return false;
        if(!t->walkableElev() && t->isElevationTile()) return false;
        return true; }, [&](eTileBase *const t)
                  { return t->x() == mPressedTX && t->y() == mPressedTY; });
    const auto startTile = mBoard->tile(mHoverTX, mHoverTY);
    const int w = mBoard->width();
    const int h = mBoard->height();
    const bool r = p.findPath({0, 0, w, h}, startTile, 100, true, w, h);
    if (!r)
        return false;
    return p.extractPath(path);
}

std::vector<eTile *> eGameWidget::roadPath() const
{
    std::vector<eOrientation> orients;
    const auto allowed = mEditorMode ? eTerrain::buildableAfterClear : eTerrain::buildable;
    ePathFinder p([allowed](eTileBase *const t)
                  {
        const auto terr = t->terrain();
        const bool tr = static_cast<bool>(allowed & terr);
        if(!tr) return false;
        const auto bt = t->underBuildingType();
        const bool r = bt == eBuildingType::road ||
                       bt == eBuildingType::none;
        if(!r) return false;
        if(!t->walkableElev() && t->isElevationTile()) return false;
        return true; }, [&](eTileBase *const t)
                  { return t->x() == mPressedTX && t->y() == mPressedTY; });
    const auto startTile = mBoard->tile(mHoverTX, mHoverTY);
    const int w = mBoard->width();
    const int h = mBoard->height();
    if (!p.findPath({0, 0, w, h}, startTile, 100, true, w, h))
        return {};
    if (!p.extractPath(orients))
        return {};
    std::vector<eTile *> tiles;
    eTile *t = startTile;
    for (int i = orients.size() - 1; i >= 0; i--)
    {
        if (!t)
            break;
        tiles.push_back(t);
        t = t->neighbour<eTile>(orients[i]);
    }
    if (t)
        tiles.push_back(t);
    return tiles;
}

bool eGameWidget::columnPath(std::vector<eOrientation> &path)
{
    ePathFinder p([](eTileBase *const t)
                  {
        const auto terr = t->terrain();
        const bool tr = static_cast<bool>(eTerrain::buildable & terr);
        if(!tr) return false;
        if(t->isElevationTile()) return false;
        const auto bt = t->underBuildingType();
        const bool r = bt == eBuildingType::doricColumn ||
                       bt == eBuildingType::ionicColumn ||
                       bt == eBuildingType::corinthianColumn ||
                       bt == eBuildingType::none;
        if(!r) return false;
        return true; }, [&](eTileBase *const t)
                  { return t->x() == mPressedTX && t->y() == mPressedTY; });
    const auto startTile = mBoard->tile(mHoverTX, mHoverTY);
    const int w = mBoard->width();
    const int h = mBoard->height();
    const bool r = p.findPath({0, 0, w, h}, startTile, 100, true, w, h);
    if (!r)
        return false;
    return p.extractPath(path);
}

bool eGameWidget::bridgeTiles(eTile *const t, const eTerrain terr,
                              std::vector<eTile *> &tiles,
                              bool &rotated)
{
    tiles.clear();
    rotated = false;
    if (!t)
        return false;
    if (!t->isShoreTile(terr))
        return false;
    if (t->underBuilding())
        return false;
    const auto tl = t->topLeft<eTile>();
    if (!tl)
        return false;
    const auto tr = t->topRight<eTile>();
    if (!tr)
        return false;
    const auto bl = t->bottomLeft<eTile>();
    if (!bl)
        return false;
    const auto br = t->bottomRight<eTile>();
    if (!br)
        return false;

    if (tr->isShoreTile(terr) && bl->isShoreTile(terr))
    {
        if (br->hasTerrain(terr))
        {
            if (tl->hasTerrain(terr))
                return false;
            auto tt = t;
            tiles.push_back(tt);
            while (true)
            {
                const auto ttt = tt->bottomRight<eTile>();
                if (!ttt || ttt->hasBridge() || !ttt->hasTerrain(terr))
                    break;
                tt = ttt;
                tiles.push_back(tt);
                if (tt->isShoreTile(terr))
                    break;
            }
            if (!tt)
                return false;
            const auto tt_tr = tt->topRight<eTile>();
            const auto tt_bl = tt->bottomLeft<eTile>();
            if (!tt_tr->isShoreTile(terr) || !tt_bl->isShoreTile(terr))
            {
                return false;
            }
            const auto tt_tl = tt->bottomRight<eTile>();
            if (tt_tl->hasTerrain(terr))
                return false;
        }
        else
        {
            auto tt = t;
            tiles.push_back(tt);
            while (true)
            {
                const auto ttt = tt->topLeft<eTile>();
                if (!ttt || ttt->hasBridge() || !ttt->hasTerrain(terr))
                    break;
                tt = ttt;
                tiles.push_back(tt);
                if (tt->isShoreTile(terr))
                    break;
            }
            if (!tt)
                return false;
            const auto tt_tr = tt->topRight<eTile>();
            const auto tt_bl = tt->bottomLeft<eTile>();
            if (!tt_tr->isShoreTile(terr) || !tt_bl->isShoreTile(terr))
            {
                return false;
            }
            const auto tt_tl = tt->topLeft<eTile>();
            if (tt_tl->hasTerrain(terr))
                return false;
        }
        return !tr->underBuilding() && !bl->underBuilding();
    }
    else if (tl->isShoreTile(terr) && br->isShoreTile(terr))
    {
        rotated = true;
        if (bl->hasTerrain(terr))
        {
            if (tr->hasTerrain(terr))
                return false;
            auto tt = t;
            tiles.push_back(tt);
            while (true)
            {
                const auto ttt = tt->bottomLeft<eTile>();
                if (!ttt || ttt->hasBridge() || !ttt->hasTerrain(terr))
                    break;
                tt = ttt;
                tiles.push_back(tt);
                if (tt->isShoreTile(terr))
                    break;
            }
            if (!tt)
                return false;
            const auto tt_tl = tt->topLeft<eTile>();
            const auto tt_br = tt->bottomRight<eTile>();
            if (!tt_tl->isShoreTile(terr) || !tt_br->isShoreTile(terr))
            {
                return false;
            }
            const auto tt_bl = tt->bottomLeft<eTile>();
            if (tt_bl->hasTerrain(terr))
                return false;
        }
        else
        {
            auto tt = t;
            tiles.push_back(tt);
            while (true)
            {
                const auto ttt = tt->topRight<eTile>();
                if (!ttt || ttt->hasBridge() || !ttt->hasTerrain(terr))
                    break;
                tt = ttt;
                tiles.push_back(tt);
                if (tt->isShoreTile(terr))
                    break;
            }
            if (!tt)
                return false;
            const auto tt_tl = tt->topLeft<eTile>();
            const auto tt_br = tt->bottomRight<eTile>();
            if (!tt_tl->isShoreTile(terr) || !tt_br->isShoreTile(terr))
            {
                return false;
            }
            const auto tt_tr = tt->topRight<eTile>();
            if (tt_tr->hasTerrain(terr))
                return false;
        }
        return !tl->underBuilding() && !br->underBuilding();
    }

    return false;
}

bool eGameWidget::canBuildAvenue(eTile *const t, const eCityId cid,
                                 const ePlayerId pid,
                                 const bool forestAllowed) const
{
    return mBoard->canBuildAvenue(t, cid, pid, forestAllowed);
}

void eGameWidget::setPatrolBuilding(ePatrolBuildingBase *const pb)
{
    mWalkerBuilding = nullptr;
    mPatrolHighlightBuilding = pb;
    if (pb)
    {
        mSavedViewMode = mViewMode;

        const auto fw = new eFramedWidget(window());
        fw->setType(eFrameType::message);
        const int p = fw->padding();
        fw->resize(60 * p, 11 * p);
        addWidget(fw);
        fw->align(eAlignment::bottom);
        fw->move((width() - mGm->width() - fw->width()) / 2, fw->y() - 2 * p);

        const auto title = new eLabel("Waypoints", window());
        title->fitContent();
        fw->addWidget(title);
        title->align(eAlignment::hcenter);
        title->setY(p);

        const auto buttons = new eWidget(window());
        buttons->resize(fw->width() - 2 * p, 5 * p);
        fw->addWidget(buttons);
        buttons->move(p, fw->height() - buttons->height() - p);

        const auto clearb = new eButton("clear", window());
        clearb->fitContent();
        clearb->setPressAction([this]()
                               {
            if(!mPatrolBuilding) return;
            auto& pgs = mPatrolBuilding->patrolGuides();
            pgs.clear();
            updatePatrolPath(); 
            setViewMode(mSavedViewMode); });
        buttons->addWidget(clearb);
        clearb->align(eAlignment::vcenter);

        const auto resetb = new eButton("restore", window());
        resetb->fitContent();
        resetb->setPressAction([this]()
                               {
            auto& pgs = mPatrolBuilding->patrolGuides();
            pgs = mSavedGuides;
            updatePatrolPath(); });
        buttons->addWidget(resetb);
        resetb->align(eAlignment::vcenter);

        const bool bd = pb->bothDirections();
        const auto bothTxt = bd ? "both ways" : "one way";
        const auto bothb = new eButton(bothTxt, window());
        bothb->fitContent();
        bothb->setPressAction([this, bothb]()
                              {
            const bool bd = mPatrolBuilding->bothDirections();
            mPatrolBuilding->setBothDirections(!bd);
            const auto bothTxt = bd ? "one way" : "both ways";
            bothb->setText(bothTxt);
            updatePatrolPath(); });
        buttons->addWidget(bothb);
        bothb->align(eAlignment::vcenter);

        const auto closeb = new eButton(eLanguage::text("close"), window());
        closeb->fitContent();
        closeb->setPressAction([this]()
                               { setPatrolBuilding(nullptr); });
        buttons->addWidget(closeb);
        closeb->align(eAlignment::vcenter);

        buttons->layoutHorizontally();

        mPatrolPathWid = fw;

        mSavedGuides = pb->patrolGuides();
    }
    else if (mViewMode == eViewMode::patrolBuilding)
    {
        setViewMode(mSavedViewMode);
    }

    if (mPatrolPathWid && !pb)
    {
        mPatrolPathWid->deleteLater();
        mPatrolPathWid = nullptr;
    }

    mPatrolBuilding = pb;
    updatePatrolPath();
}

bool eGameWidget::inErase(const int tx, const int ty)
{
    const auto mode = mGm->mode();
    const bool e = mode == eBuildingMode::erase;
    const bool high = mTem->visible() || e;
    if (!high)
        return false;

    const int sMinX = std::min(mPressedTX, mHoverTX);
    const int sMinY = std::min(mPressedTY, mHoverTY);
    const int sMaxX = std::max(mPressedTX, mHoverTX);
    const int sMaxY = std::max(mPressedTY, mHoverTY);

    bool s = false;
    if (mLeftPressed &&
        tx >= sMinX && tx <= sMaxX &&
        ty >= sMinY && ty <= sMaxY)
    {
        s = true;
    }
    const bool h = tx == mHoverTX && ty == mHoverTY;
    return h || s;
}

bool eGameWidget::inErase(const SDL_Rect &rect)
{
    for (int x = rect.x; x < rect.x + rect.w; x++)
    {
        for (int y = rect.y; y < rect.y + rect.h; y++)
        {
            const bool r = inErase(x, y);
            if (r)
                return true;
        }
    }
    return false;
}

eBuilding* eGameWidget::eraseBuildingAt(const int tx, const int ty) const
{
    if(const auto b = mBoard->buildingAt(tx, ty)) return b;
    return nullptr;
}

eTile* eGameWidget::eraseParkParentTileAt(const int tx, const int ty) const
{
    const auto tile = mBoard->tile(tx, ty);
    if(!tile) return nullptr;
    if(tile->underBuildingType() != eBuildingType::park) return nullptr;
    if(const auto parent = tile->underTile()) {
        if(parent->underBuildingType() == eBuildingType::park) return parent;
    }
    return tile;
}

bool eGameWidget::inErase(eAgoraBase *const a)
{
    if(!a) return false;
    const auto rr = a->tileRect();
    for (int x = rr.x; x < rr.x + rr.w; x++)
    {
        for (int y = rr.y; y < rr.y + rr.h; y++)
        {
            if (!inErase(x, y))
                continue;
            const auto t = mBoard->tile(x, y);
            if (!t)
                continue;
            const auto ub = t->underBuilding();
            if (!ub)
                continue;
            const auto v = dynamic_cast<eVendor *>(ub);
            if (!v)
                return true;
        }
    }
    return false;
}

bool eGameWidget::inErase(eBuilding *const b)
{
    if (!b)
        return false;
    const auto mode = mGm->mode();
    const bool e = mode == eBuildingMode::erase;
    const bool high = mTem->visible() || e;
    if (!high)
        return false;

    if(b->type() == eBuildingType::park) {
        if(mLeftPressed) {
            const auto tile = b->centerTile();
            if(!tile) return false;
            return inErase(tile->x(), tile->y());
        }
        const auto parent = eraseParkParentTileAt(mHoverTX, mHoverTY);
        if(!parent) return false;
        const auto tile = b->centerTile();
        if(!tile) return false;
        return tile == parent || tile->underTile() == parent;
    }

    SDL_Rect rect;
    if (const auto sb = dynamic_cast<eSanctBuilding *>(b))
    {
        const auto s = sb->monument();
        rect = s->tileRect();
    }
    else if (const auto v = dynamic_cast<eVendor *>(b))
    {
        if (inErase(b->tileRect()))
            return true;
        const auto a = v->agora();
        return a && inErase(a);
    }
    else if (const auto as = dynamic_cast<eAgoraSpace *>(b))
    {
        const auto a = as->agora();
        return inErase(a);
    }
    else if (const auto r = dynamic_cast<eRoad *>(b))
    {
        if (r->isBridge())
        {
            const auto t = r->centerTile();
            if (t)
            {
                std::vector<eTile *> tiles;
                r->bridgeConnectedTiles(tiles);
                for (const auto t : tiles)
                {
                    const bool r = inErase(t->x(), t->y());
                    if (r)
                        return true;
                }
                return false;
            }
            return false;
        }
        else
        {
            if (const auto a = r->underAgora())
            {
                return inErase(a);
            }
            else if (const auto g = r->underGatehouse())
            {
                return inErase(g);
            }
            else
                rect = b->tileRect();
        }
    }
    else if (const auto a = dynamic_cast<eAgoraBase *>(b))
    {
        return inErase(a);
    }
    else if (const auto p = dynamic_cast<ePalace *>(b))
    {
        const auto &ts = p->tiles();
        for (const auto &t : ts)
        {
            const auto tt = t->centerTile();
            const int tx = tt->x();
            const int ty = tt->y();
            if (inErase(tx, ty))
                return true;
        }
        rect = p->tileRect();
    }
    else if (const auto p = dynamic_cast<ePalaceTile *>(b))
    {
        return inErase(p->palace());
    }
    else if (const auto p = dynamic_cast<eGodMonument *>(b))
    {
        const auto &ts = p->tiles();
        for (const auto &t : ts)
        {
            const auto tt = t->centerTile();
            const int tx = tt->x();
            const int ty = tt->y();
            if (inErase(tx, ty))
                return true;
        }
        rect = p->tileRect();
    }
    else if (const auto t = dynamic_cast<eGodMonumentTile *>(b))
    {
        return inErase(t->monument());
    }
    else if (const auto hr = dynamic_cast<eHorseRanch *>(b))
    {
        const bool e1 = inErase(hr->tileRect());
        if (e1)
            return true;
        const auto hre = hr->enclosure();
        rect = hre->tileRect();
    }
    else if (const auto hr = dynamic_cast<eHorseRanchEnclosure *>(b))
    {
        const bool e1 = inErase(hr->tileRect());
        if (e1)
            return true;
        const auto hre = hr->ranch();
        rect = hre->tileRect();
    }
    else if (const auto tp = dynamic_cast<eTradePost *>(b))
    {
        const bool e1 = inErase(tp->tileRect());
        if (e1)
            return true;
        const auto ub = tp->unpackBuilding();
        if (!ub)
            return false;
        rect = ub->tileRect();
    }
    else if (const auto p = dynamic_cast<ePier *>(b))
    {
        const bool e1 = inErase(p->tileRect());
        if (e1)
            return true;
        const auto tp = p->tradePost();
        if (!tp)
            return false;
        rect = tp->tileRect();
    }
    else
    {
        rect = b->tileRect();
    }
    return inErase(rect);
}

bool eGameWidget::inRepair(const int tx, const int ty)
{
    const auto mode = mGm->mode();
    const bool r = mode == eBuildingMode::repair;
    if (!r)
        return false;

    const int sMinX = std::min(mPressedTX, mHoverTX);
    const int sMinY = std::min(mPressedTY, mHoverTY);
    const int sMaxX = std::max(mPressedTX, mHoverTX);
    const int sMaxY = std::max(mPressedTY, mHoverTY);

    bool s = false;
    if (mLeftPressed &&
        tx >= sMinX && tx <= sMaxX &&
        ty >= sMinY && ty <= sMaxY)
    {
        s = true;
    }
    const bool h = tx == mHoverTX && ty == mHoverTY;
    return h || s;
}

bool eGameWidget::inPatrolBuildingHover(eBuilding *const b)
{
    const auto mode = mGm->mode();
    const bool e = mode == eBuildingMode::none;
    if (!e)
        return false;
    if (!b)
        return false;
    const auto cid = b->cityId();
    const auto pid = mBoard->cityIdToPlayerId(cid);
    const auto ppid = mBoard->personPlayer();
    if (pid != ppid)
        return false;
    if (const auto pb = dynamic_cast<ePatrolBuildingBase *>(b))
    {
        if (!pb->spawnsPatrolers())
            return false;
        const auto r = pb->tileRect();
        const SDL_Point hover{mHoverTX, mHoverTY};
        const bool hovered = SDL_PointInRect(&hover, &r);
        return hovered;
    }
    else if (const auto as = dynamic_cast<eAgoraSpace *>(b))
    {
        return inPatrolBuildingHover(as->agora());
    }
    else if (const auto v = dynamic_cast<eVendor *>(b))
    {
        const auto r = v->tileRect();
        const SDL_Point hover{mHoverTX, mHoverTY};
        return SDL_PointInRect(&hover, &r);
    }
    return false;
}

void eGameWidget::switchPause()
{
    mPaused = !mPaused;
    if (mPaused && !mPausedLabel)
    {
        const auto hotkeyName = SDL_GetScancodeName(window()->settings().fHotkeyPause);
        const auto str = std::string("Game Paused '") + hotkeyName + "' continues";
        const auto space = "     ";
        mPausedLabel = new eFramedLabel(space + str + space, window());
        mPausedLabel->setType(eFrameType::message);
        mPausedLabel->setFontSizeS();
        mPausedLabel->setPaddingXL();
        mPausedLabel->fitContent();
        addWidget(mPausedLabel);
        const int vw = width() - mGm->width();
        const int w = mPausedLabel->width();
        mPausedLabel->setX((vw - w) / 2);
        const int p = mPausedLabel->padding();
        mPausedLabel->setY(mTopBar->height() + 2 * p);
        updateTipPositions();
        updateToastPositions();
    }
    else if (mPausedLabel)
    {
        mPausedLabel->deleteLater();
        mPausedLabel = nullptr;
        updateTipPositions();
    }
    updateTipPositions();
    updateToastPositions();
}

void eGameWidget::setSpeedId(const int id)
{
    mSpeedId = std::clamp(id, 0, sMaxSpeedId);
    mSpeed = sSpeeds[mSpeedId];
}

void eGameWidget::showSpeedLabel()
{
    const auto text = std::string("Game speed: ") + sSpeedLabels[mSpeedId];
    if (!mSpeedLabel)
    {
        mSpeedLabel = new eFramedLabel(text, window());
        mSpeedLabel->setType(eFrameType::message);
        mSpeedLabel->setFontSizeS();
        mSpeedLabel->setPaddingXL();
        addWidget(mSpeedLabel);
    }
    else
    {
        mSpeedLabel->setText(text);
    }
    mSpeedLabel->fitContent();
    const int vw = width() - mGm->width();
    mSpeedLabel->setX((vw - mSpeedLabel->width()) / 2);
    const int p = mSpeedLabel->padding();
    int y = mTopBar->height() + 2 * p;
    if (mPausedLabel)
    {
        y = mPausedLabel->y() + mPausedLabel->height() + 2 * p;
    }
    mSpeedLabel->setY(y);
    updateTipPositions();
    updateToastPositions();
    mSpeedLabelHideFrame = mFrame + 120;
}

bool eGameWidget::keyPressEvent(const eKeyPressEvent &e)
{
    if (mLocked)
        return true;
    if (mStampTemplateNameDialogOpen)
        return true;
    const auto k = e.key();
    const auto &hotkeys = window()->settings();
    if (updateSmoothScrollKey(k, true))
        return true;
    if (k == hotkeys.fHotkeySpeedUp ||
        k == SDL_Scancode::SDL_SCANCODE_KP_PLUS)
    {
        const int oldSpeedId = mSpeedId;
        setSpeedId(mSpeedId + 1);
        if (mSpeedId != oldSpeedId)
            showSpeedLabel();
    }
    else if (k == hotkeys.fHotkeySpeedDown ||
             k == SDL_Scancode::SDL_SCANCODE_KP_MINUS)
    {
        const int oldSpeedId = mSpeedId;
        setSpeedId(mSpeedId - 1);
        if (mSpeedId != oldSpeedId)
            showSpeedLabel();
    }
    else if (k == hotkeys.fHotkeyRotatePreview && e.shiftPressed())
    {
        if (mGm->mode() == eBuildingMode::stamp)
            mStampTool->setMirror(1 - mStampTool->mirror());
    }
    else if (k == hotkeys.fHotkeyRotatePreview)
    {
        mRotate = !mRotate;
        mRotateFrame = (mRotateFrame / gRotateFrames + 1) * gRotateFrames;
        mRotateId++;
        if (mRotateId > 3)
            mRotateId = 0;
        if (mGm->mode() == eBuildingMode::stamp)
            mStampTool->setRotation(mStampTool->rotation() + 1);
    }
    else if (k == hotkeys.fHotkeyPause)
    {
        if (!mMsgBox)
            switchPause();
    }
    else if (k == hotkeys.fHotkeyCopyBuilding)
    {
        selectHoveredBuildingMode();
    }
    else if (k == hotkeys.fHotkeyDeleteTool)
    {
        mGm->setMode(eBuildingMode::erase);
    }
    else if (k == hotkeys.fHotkeyRepairTool)
    {
        mGm->setMode(eBuildingMode::repair);
    }
    else if (k == hotkeys.fHotkeyUndo)
    {
        mBoard->undoLastAction();
        mGm->update();
    }
    else if (k == hotkeys.fHotkeyShowRoadsOverlay)
    {
        toggleViewMode(eViewMode::roads);
    }
    else if (k == hotkeys.fHotkeyBuildRoad)
    {
        mGm->setMode(eBuildingMode::road);
    }
    else if (k == hotkeys.fHotkeyBuildRoadblock)
    {
        mGm->setMode(eBuildingMode::roadblock);
    }
    else if (k == hotkeys.fHotkeyBuildMaintenanceOffice)
    {
        mGm->setMode(eBuildingMode::maintenanceOffice);
    }
    else if (k == hotkeys.fHotkeyBuildCommonHousing)
    {
        mGm->setMode(eBuildingMode::commonHousing);
    }
    else if (k == hotkeys.fHotkeyBuildWatchpost)
    {
        mGm->setMode(eBuildingMode::watchpost);
    }
    else if (k == hotkeys.fHotkeyBuildStamp)
    {
        mGm->setMode(eBuildingMode::stamp);
    }
    else if (k == hotkeys.fHotkeyStampManager)
    {
        showStampManager();
    }
    else if (k == hotkeys.fHotkeyBookmark1)
    {
        if (e.ctrlPressed())
        {
            setBookmark(1);
        }
        else
        {
            viewBookmark(1);
        }
    }
    else if (k == hotkeys.fHotkeyBookmark2)
    {
        if (e.ctrlPressed())
        {
            setBookmark(2);
        }
        else
        {
            viewBookmark(2);
        }
    }
    else if (k == hotkeys.fHotkeyBookmark3)
    {
        if (e.ctrlPressed())
        {
            setBookmark(3);
        }
        else
        {
            viewBookmark(3);
        }
    }
    else if (k == hotkeys.fHotkeyBookmark4)
    {
        if (e.ctrlPressed())
        {
            setBookmark(4);
        }
        else
        {
            viewBookmark(4);
        }
    }
    else if (k == hotkeys.fHotkeyMenuTab1)
    {
        mGm->selectTab(0);
    }
    else if (k == hotkeys.fHotkeyMenuTab2)
    {
        mGm->selectTab(1);
    }
    else if (k == hotkeys.fHotkeyMenuTab3)
    {
        mGm->selectTab(2);
    }
    else if (k == hotkeys.fHotkeyMenuTab4)
    {
        mGm->selectTab(3);
    }
    else if (k == hotkeys.fHotkeyMenuTab5)
    {
        mGm->selectTab(4);
    }
    else if (k == hotkeys.fHotkeyMenuTab6)
    {
        mGm->selectTab(5);
    }
    else if (k == hotkeys.fHotkeyMenuTab7)
    {
        mGm->selectTab(6);
    }
    else if (k == hotkeys.fHotkeyMenuTab8)
    {
        mGm->selectTab(7);
    }
    else if (k == hotkeys.fHotkeyMenuTab9)
    {
        mGm->selectTab(8);
    }
    else if (k == hotkeys.fHotkeyMenuTab10)
    {
        mGm->selectTab(9);
    }
    else if (k == hotkeys.fHotkeyMenuTab11)
    {
        mGm->selectTab(10);
    }
    else if (k == hotkeys.fHotkeyGameMenu)
    {
        if (!mMsgBox && !mBoard->editorMode())
        {
            mBoard->waitUntilFinished();
            if (mGm->mode() != eBuildingMode::none)
                mGm->clearMode();
            const auto menu = new eGameMainMenu(window());
            menu->resize(width() / 4, height() / 2);
            const auto w = window();
            const bool wasPaused = mPaused;
            if (!mPaused)
                switchPause();
            const auto resumeAct = [this, wasPaused, menu]()
            {
                if (!wasPaused)
                    switchPause();
                menu->deleteLater();
            };
            const auto saveAct = [this, w]()
            {
                mLocked = true;
                const auto fw = new eLoadGame(w);
                const auto func = [w](const std::string &path)
                {
                    return w->saveGame(path);
                };
                const auto closeAct = [this, fw]()
                {
                    mLocked = false;
                    removeWidget(fw);
                    fw->deleteLater();
                };
                const auto dir = w->leaderSaveDir();
                fw->intialize(eLanguage::zeusText(1, 4),
                              dir, func, closeAct);
                addWidget(fw);
                fw->align(eAlignment::center);
                w->execDialog(fw);
            };
            const auto loadAct = [this, w]()
            {
                mLocked = true;
                const auto fw = new eLoadGame(w);
                const auto func = [w, fw](const std::string &path)
                {
                    fw->deleteLater();
                    w->addSlot([w, path]()
                               {
                        w->closeGame();
                        w->loadGame(path); });
                    return true;
                };
                const auto closeAct = [this, fw]()
                {
                    mLocked = false;
                    fw->deleteLater();
                };
                const auto dir = w->leaderSaveDir();
                fw->intialize(eLanguage::zeusText(1, 3),
                              dir, func, closeAct);
                w->execDialog(fw, true, closeAct);
            };
            const auto exitAct = [w]() { // "Exit game" button in in-game options popup
                w->addSlot([w]()
                           { w->closeGame(); });
            };
            const auto optionsAct = [this]()
            {
                showOptionsMenu();
            };
            stopSmoothScroll();
            const auto closeMenu = [this, wasPaused, menu]()
            {
                if (!wasPaused)
                    switchPause();
                menu->deleteLater();
            };
            menu->initialize(resumeAct, saveAct, loadAct, optionsAct,
                             exitAct);
            addWidget(menu);
            menu->align(eAlignment::center);
            w->execDialog(menu, true, closeMenu);
        }
    }
    return true;
}

bool eGameWidget::keyReleaseEvent(const eKeyPressEvent &e)
{
    updateSmoothScrollKey(e.key(), false);
    return true;
}

bool eGameWidget::updateSmoothScrollKey(const SDL_Scancode k,
                                        const bool pressed)
{
    const auto &hotkeys = window()->settings();
    if (k == hotkeys.fHotkeyScrollLeft ||
        k == SDL_Scancode::SDL_SCANCODE_LEFT)
    {
        if (pressed)
            mSmoothScrollX = 1;
        else
            updateSmoothScrollKeysPressed();
        return true;
    }
    else if (k == hotkeys.fHotkeyScrollRight ||
             k == SDL_Scancode::SDL_SCANCODE_RIGHT)
    {
        if (pressed)
            mSmoothScrollX = -1;
        else
            updateSmoothScrollKeysPressed();
        return true;
    }
    else if (k == hotkeys.fHotkeyScrollUp ||
             k == SDL_Scancode::SDL_SCANCODE_UP)
    {
        if (pressed)
            mSmoothScrollY = 1;
        else
            updateSmoothScrollKeysPressed();
        return true;
    }
    else if (k == hotkeys.fHotkeyScrollDown ||
             k == SDL_Scancode::SDL_SCANCODE_DOWN)
    {
        if (pressed)
            mSmoothScrollY = -1;
        else
            updateSmoothScrollKeysPressed();
        return true;
    }
    return false;
}

bool eGameWidget::smoothScrollKeyPressed(const SDL_Scancode k) const
{
    if (k == SDL_Scancode::SDL_SCANCODE_UNKNOWN)
        return false;
    int keyCount = 0;
    const auto keys = SDL_GetKeyboardState(&keyCount);
    const int keyId = static_cast<int>(k);
    return keyId >= 0 && keyId < keyCount && keys[keyId];
}

void eGameWidget::updateSmoothScrollKeysPressed()
{
    if(mLocked) return;
    if(mStampTemplateNameDialogOpen) {
        mSmoothScrollX = 0;
        mSmoothScrollY = 0;
        return;
    }
    const auto &hotkeys = window()->settings();
    const bool scrollLeft = smoothScrollKeyPressed(hotkeys.fHotkeyScrollLeft) ||
                            smoothScrollKeyPressed(SDL_Scancode::SDL_SCANCODE_LEFT);
    const bool scrollRight = smoothScrollKeyPressed(hotkeys.fHotkeyScrollRight) ||
                             smoothScrollKeyPressed(SDL_Scancode::SDL_SCANCODE_RIGHT);
    const bool scrollUp = smoothScrollKeyPressed(hotkeys.fHotkeyScrollUp) ||
                          smoothScrollKeyPressed(SDL_Scancode::SDL_SCANCODE_UP);
    const bool scrollDown = smoothScrollKeyPressed(hotkeys.fHotkeyScrollDown) ||
                            smoothScrollKeyPressed(SDL_Scancode::SDL_SCANCODE_DOWN);
    if (mSmoothScrollX == 1 && !scrollLeft)
        mSmoothScrollX = scrollRight ? -1 : 0;
    else if (mSmoothScrollX == -1 && !scrollRight)
        mSmoothScrollX = scrollLeft ? 1 : 0;
    else if (mSmoothScrollX == 0)
        mSmoothScrollX = scrollLeft ? 1 : (scrollRight ? -1 : 0);
    if (mSmoothScrollY == 1 && !scrollUp)
        mSmoothScrollY = scrollDown ? -1 : 0;
    else if (mSmoothScrollY == -1 && !scrollDown)
        mSmoothScrollY = scrollUp ? 1 : 0;
    else if (mSmoothScrollY == 0)
        mSmoothScrollY = scrollUp ? 1 : (scrollDown ? -1 : 0);
}

void eGameWidget::smoothScroll()
{
    updateSmoothScrollKeysPressed();
    const double scale = mLastDtMs / kBaseRenderMs;
    const double d = mKeyScrollSpeed * scale;
    const int dx = int(std::lround(mSmoothScrollX * d));
    const int dy = int(std::lround(mSmoothScrollY * d));
    if (dx)
        setDX(mDX + dx);
    if (dy)
        setDY(mDY + dy);
}

void eGameWidget::setKeyScrollSpeed(const int speed)
{
    mKeyScrollSpeed = eSettings::clampKeyScrollSpeed(speed);
    window()->setKeyScrollSpeed(mKeyScrollSpeed * 10);
}

void eGameWidget::updateKeyScrollSpeed(const int speed)
{
    mKeyScrollSpeed = speed * 5;
}

void eGameWidget::stopSmoothScroll()
{
    mSmoothScrollX = 0;
    mSmoothScrollY = 0;
}

bool eGameWidget::mousePressEvent(const eMouseEvent &e)
{
    mPressedButtons = mPressedButtons | e.button();
    if (mLocked)
        return true;
    mGm->closeBuildWidget();
    mMovedSincePress = false;
    const auto b = e.button();
    if (b != eMouseButton::middle)
    {
        eSounds::playButtonSound();
    }
    switch (b)
    {
    case eMouseButton::middle:
        mLastX = e.x();
        mLastY = e.y();
        return true;
    case eMouseButton::left:
    {
        if (hasInfoWidget())
            return true;
        mLeftPressed = true;
        int tx;
        int ty;
        pixToId(e.x(), e.y(), tx, ty);
        mPressedTX = tx;
        mPressedTY = ty;
        mPressedX = e.x();
        mPressedY = e.y();
        const auto tile = mBoard->tile(tx, ty);
        if (mCreatingStampTemplate)
        {
            mHoverTX = tx;
            mHoverTY = ty;
            updateStampTemplateSelection();
            return true;
        }
        if (mTem->visible())
        {
            mInflTiles = mHoverTiles;
        }
        if (mPatrolBuilding)
        {
            if (!tile)
                return true;
            auto &pgs = mPatrolBuilding->patrolGuides();
            const auto it = findGuide(tx, ty);
            if (it != pgs.end())
            {
                pgs.erase(it);
            }
            else
            {
                if (tile->hasRoad())
                {
                    if (mViewMode != eViewMode::patrolBuilding)
                        setViewMode(eViewMode::patrolBuilding);
                    pgs.push_back({tx, ty});
                }
                else
                {
                    setPatrolBuilding(nullptr);
                }
            }
            if (mPatrolBuilding)
                updatePatrolPath();
        }
    }
        return true;
    case eMouseButton::right:
    {
        mRightPressed = true;
        mRightPanning = false;
        mLastX = e.x();
        mLastY = e.y();
        mPressedX = e.x();
        mPressedY = e.y();
        pixToId(e.x(), e.y(), mPressedTX, mPressedTY);
        return true;
    }
    break;
    default:
        return true;
    }
    return true;
}

bool eGameWidget::rightClickRelease(const eMouseEvent &e)
{
    for (const auto w : children())
    {
        const auto d = dynamic_cast<eModal *>(w);
        if (d && d->visible())
        {
            d->close();
            return true;
        }
    }
    if (mGm->mode() != eBuildingMode::none)
    {
        mGm->clearMode();
        return true;
    }
    const auto &solds = mBoard->selectedSoldiers();
    if (!solds.empty())
        return false;
    const auto &trims = mBoard->selectedTriremes();
    if (!trims.empty())
        return false;

    if (mPatrolBuilding)
    {
        setPatrolBuilding(nullptr);
        return true;
    }
    int tx;
    int ty;
    pixToId(e.x(), e.y(), tx, ty);
    const auto tile = mBoard->tile(tx, ty);
    if (mCreatingStampTemplate)
    {
        cancelStampTemplateCreate();
        return true;
    }
    if (!tile)
        return true;
    const auto b = tile->underBuilding();
    const auto chars = tile->characters();
    std::vector<eCharacter *> chars2;
    for (const auto &c : chars)
    {
        const auto type = c->type();
        if (type == eCharacterType::trailer)
            continue;
        if (c->dead())
            continue;
        chars2.push_back(c.get());
    }
    if (!chars2.empty() && (!b || eBuilding::sFlatBuilding(b->type())))
    {
        openInfoWidget(chars2);
    }
    else if (b)
    {
        if (b->type() == eBuildingType::road)
            return true;
        eSounds::playSoundForBuilding(b);
        const auto cid = tile->cityId();
        const auto pid = mBoard->cityIdToPlayerId(cid);
        const auto ppid = mBoard->personPlayer();
        if (pid != ppid && !mBoard->editorMode())
            return true;
        openInfoWidget(b);
    }
    return true;
}

bool eGameWidget::selectedPlayerBanners() const
{
    if (!mBoard) return false;
    const auto ppid = mBoard->personPlayer();
    for (const auto b : mBoard->selectedSoldiers())
    {
        if (b && b->playerId() == ppid)
            return true;
    }
    return false;
}

namespace {
void snapBiased8Way(const int rawDX, const int rawDY,
                    int& dx, int& dy)
{
    const int ax = std::abs(rawDX);
    const int ay = std::abs(rawDY);
    dx = rawDX == 0 ? 0 : (rawDX > 0 ? 1 : -1);
    dy = rawDY == 0 ? 0 : (rawDY > 0 ? 1 : -1);

    if(ax == 0 || ay == 0) return;

    const bool xDominant = ax > ay;
    const int major = xDominant ? ax : ay;
    const int minor = xDominant ? ay : ax;

    // Iso tile drags hit diagonals very easily; require a near-even drag
    // before snapping to a diagonal formation axis.
    if(minor*3 < major*2) {
        if(xDominant) {
            dy = 0;
        } else {
            dx = 0;
        }
    }
}

int snappedDragFacing(const int dx, const int dy)
{
    if(dx < 0 && dy < 0) return 0;
    if(dx == 0 && dy < 0) return 45;
    if(dx > 0 && dy < 0) return 90;
    if(dx > 0 && dy == 0) return 135;
    if(dx > 0 && dy > 0) return 180;
    if(dx == 0 && dy > 0) return 225;
    if(dx < 0 && dy > 0) return 270;
    if(dx < 0 && dy == 0) return 315;
    return 0;
}
}

int eGameWidget::rightDragFacing() const
{
    int lineDX;
    int lineDY;
    rightDragFormationLine(lineDX, lineDY);
    return snappedDragFacing(-lineDY, lineDX);
}

void eGameWidget::rightDragFormationLine(int& dx, int& dy) const
{
    const int dragDX = mHoverTX - mPressedTX;
    const int dragDY = mHoverTY - mPressedTY;
    const int perpDX = dragDY;
    const int perpDY = -dragDX;
    snapBiased8Way(perpDX, perpDY, dx, dy);
    if(dx == 0 && dy == 0)
    {
        const int pixDX = mHoverX - mPressedX;
        const int pixDY = mHoverY - mPressedY;
        snapBiased8Way(pixDY, -pixDX, dx, dy);
    }
    if(dx == 0 && dy == 0)
    {
        dx = 1;
        dy = 0;
    }
}

void brushTiles(eGameBoard *const board, const int bSize,
                const int cx, const int cy,
                std::vector<eTile *> &result)
{
    int cdx0;
    int cdy0;
    eTileHelper::tileIdToDTileId(cx, cy, cdx0, cdy0);
    const int x0 = cx - bSize + 1;
    const int y0 = cy;
    int dx0;
    int dy0;
    eTileHelper::tileIdToDTileId(x0, y0, dx0, dy0);
    for (int ddy = 0; ddy < 2 * bSize - 1; ddy++)
    {
        const int dy = dy0 + ddy;
        const int w = (ddy % 2) ? bSize - 1 : bSize;
        int dx = dx0;
        if (ddy % 2)
        {
            if (cdy0 % 2 == bSize % 2)
            {
                dx += 1;
            }
        }
        for (int ddx = 0; ddx < w; ddx++)
        {
            const auto t = board->dtile(dx + ddx, dy);
            if (!t)
                continue;
            result.push_back(t);
        }
    }
}

void squareTiles(eGameBoard *const board, const int bSize,
                 const int cx, const int cy,
                 std::vector<eTile *> &result)
{
    const int x0 = cx - bSize / 2;
    const int y0 = cy - bSize / 2;
    for (int dx = 0; dx < bSize; dx++)
    {
        for (int dy = 0; dy < bSize; dy++)
        {
            const int x = x0 + dx;
            const int y = y0 + dy;
            const auto t = board->tile(x, y);
            if (!t)
                continue;
            result.push_back(t);
        }
    }
}

bool eGameWidget::mouseMoveEvent(const eMouseEvent &e)
{
    mHoverTiles.clear();
    if (mTem->visible())
    {
        const auto btype = mTem->brushType();
        const int bsize = mTem->brushSize();
        if (btype == eBrushType::brush)
        {
            brushTiles(mBoard, bsize, mHoverTX, mHoverTY, mHoverTiles);
        }
        else if (btype == eBrushType::square)
        {
            squareTiles(mBoard, bsize, mHoverTX, mHoverTY, mHoverTiles);
        }
    }
    if (mLocked)
        return true;
    mMovedSincePress = true;
    const bool middle = static_cast<bool>(e.buttons() & eMouseButton::middle);
    const bool right = static_cast<bool>(e.buttons() & eMouseButton::right);
    if (right && mRightPressed && !mRightPanning)
    {
        const int dx = e.x() - mPressedX;
        const int dy = e.y() - mPressedY;
        if (selectedPlayerBanners())
        {
            mRightFormationFacing = std::abs(dx) > 12 || std::abs(dy) > 12;
        }
        else
        {
            mRightPanning = std::abs(dx) > 3 || std::abs(dy) > 3;
        }
    }
    if (middle || (right && mRightPanning))
    {
        const int dx = e.x() - mLastX;
        const int dy = e.y() - mLastY;
        setDX(mDX + dx);
        setDY(mDY + dy);
        updateMinimap();
        mLastX = e.x();
        mLastY = e.y();
    }
    else
    {
        mHoverX = e.x();
        mHoverY = e.y();
        pixToId(e.x(), e.y(), mHoverTX, mHoverTY);
        const bool left = static_cast<bool>(e.buttons() & eMouseButton::left);

        if (mGm->mode() == eBuildingMode::stamp) {
            mHoverTiles.clear();
            for (const auto& elem : mStampTool->transformedBlueprint()) {
                const int tx = mHoverTX + elem.dx;
                const int ty = mHoverTY + elem.dy;
                const auto tile = mBoard->tile(tx, ty);
                if (tile) mHoverTiles.push_back(tile);
            }
        }

        if (mCreatingStampTemplate)
        {
            if (left) updateStampTemplateSelection();
            return true;
        }

        if (left && mTem->visible())
        {
            //            const auto btype = mTem->brushType();
            //            if(btype == eBrushType::apply) return true;
            //            const auto apply = editFunc();
            //            if(!apply) return true;
            for (const auto t : mHoverTiles)
            {
                const bool r = eVectorHelpers::contains(mInflTiles, t);
                if (r)
                    continue;
                //                apply(t);
                mInflTiles.push_back(t);
            }
            //            mBoard->updateMarbleTiles();
            //            mBoard->scheduleTerrainUpdate();
        }
    }
    return true;
}

bool eGameWidget::mouseReleaseEvent(const eMouseEvent &e)
{
    const auto pressedButtons = mPressedButtons;
    mPressedButtons = e.buttons();
    if (mLocked)
        return true;
    switch (e.button())
    {
    case eMouseButton::left:
    {
        mBoard->clearBannerSelection();
        mBoard->clearTriremeSelection();
        mLeftPressed = false;
        if (mCreatingStampTemplate)
        {
            updateStampTemplateSelection();
            mPressedTX = -1;
            mPressedTY = -1;
            return true;
        }
        const bool r = buildMouseRelease();
        mGm->update();
        if (!r && mGm->mode() == eBuildingMode::none)
        {
            if (mMovedSincePress)
            {
                const auto selected = selectedTiles();
                for (const auto tile : selected)
                {
                    const auto b = tile->soldierBanner();
                    if (b && !b->selected())
                    {
                        mBoard->selectBanner(b);
                    }
                    for (const auto &c : tile->characters())
                    {
                        const auto type = c->type();
                        if (type != eCharacterType::trireme)
                            continue;
                        const auto t = static_cast<eTrireme *>(c.get());
                        if (t->selected())
                            continue;
                        if (!t->selectable())
                            continue;
                        mBoard->selectTrireme(t);
                    }
                }
            }
            else
            {
                const auto tile = mBoard->tile(mHoverTX, mHoverTY);
                mWalkerBuilding = nullptr;
                mPatrolHighlightBuilding = nullptr;
                if (tile && tile->soldierBanner())
                {
                    const auto sb = tile->soldierBanner();
                    if (!sb->selected())
                        mBoard->selectBanner(sb);
                }
                else if (!mPatrolBuilding && tile)
                {
                    if (const auto b = tile->underBuilding())
                    {
                        eSounds::playSoundForBuilding(b);
                        const auto cid = tile->cityId();
                        const auto pid = mBoard->cityIdToPlayerId(cid);
                        const auto ppid = mBoard->personPlayer();
                        if (pid == ppid || mBoard->editorMode())
                        {
                            if (const auto a = dynamic_cast<eAgoraBase *>(b))
                            {
                                setPatrolBuilding(a);
                            }
                            else if (const auto r = dynamic_cast<eRoad *>(b))
                            {
                                if (const auto a = r->underAgora())
                                    setPatrolBuilding(a);
                            }
                            else if (const auto pb = dynamic_cast<ePatrolBuilding *>(b))
                            {
                                if (pb->spawnsPatrolers())
                                    setPatrolBuilding(pb);
                            }
                            else if (const auto v = dynamic_cast<eVendor *>(b))
                            {
                                mPatrolHighlightBuilding = v->agora();
                                mWalkerBuilding = v;
                            }
                            else if (const auto s = dynamic_cast<eAgoraSpace *>(b))
                            {
                                setPatrolBuilding(s->agora());
                            }
                            else if (b->spawnsCartWalkers())
                            {
                                mWalkerBuilding = b;
                            }
                        }
                    }
                }
                if (tile)
                {
                    for (const auto &c : tile->characters())
                    {
                        const auto type = c->type();
                        if (type != eCharacterType::trireme)
                            continue;
                        const auto t = static_cast<eTrireme *>(c.get());
                        if (t->selected())
                            continue;
                        if (!t->selectable())
                            continue;
                        mBoard->selectTrireme(t);
                    }
                }
            }
        }
        mPressedTX = -1;
        mPressedTY = -1;
    }
    break;
    case eMouseButton::right:
    {
        const bool wasPanning = mRightPanning;
        const bool wasFormationFacing = mRightFormationFacing;
        mRightPressed = false;
        mRightPanning = false;
        mRightFormationFacing = false;
        pixToId(e.x(), e.y(), mHoverTX, mHoverTY);
        mHoverX = e.x();
        mHoverY = e.y();
        if (wasFormationFacing)
        {
            const auto tile = mBoard->tile(mPressedTX, mPressedTY);
            if (tile && tile->cityId() == mViewedCityId)
            {
                int lineDX;
                int lineDY;
                rightDragFormationLine(lineDX, lineDY);
                eSoldierBanner::sPlacePlayerBannersFacing(
                    mBoard->selectedSoldiers(), mBoard->personPlayer(),
                    mPressedTX, mPressedTY, *mBoard, rightDragFacing(),
                    lineDX, lineDY, 3, 2);
            }
            return true;
        }
        if (wasPanning)
            return true;
        if (mEditorMode)
        {
            const auto tile = mBoard->tile(mHoverTX, mHoverTY);
            if (tile)
                tile->removeAllBanners();
        }
        const bool handled = rightClickRelease(e);
        if (handled)
            return true;
        if (static_cast<bool>(pressedButtons & eMouseButton::right))
        {
            const auto tile = mBoard->tile(mHoverTX, mHoverTY);
            if (tile && tile->cityId() == mViewedCityId)
            {
                const auto &solds = mBoard->selectedSoldiers();
                eSoldierBanner::sPlaceFacing(solds, mHoverTX, mHoverTY, *mBoard, 180, 1, 0, 3, 2);
                const auto &trims = mBoard->selectedTriremes();
                eTrireme::sPlace(trims, mHoverTX, mHoverTY, *mBoard, 3, 2);
            }
        }
    }
    break;
    default:
        return false;
    }

    return true;
}

bool eGameWidget::mouseWheelEvent(const eMouseWheelEvent &e)
{
    if (mLocked)
        return true;
    const bool wheel = std::abs(mWheel) > 0.5;
    if (!wheel)
    {
        mWheel += e.dy();
        return true;
    }
    mWheel = 0;
    const auto &sett = window()->settings();
    std::vector<eTileSize> sizes;
    int currSize = 0;
    if (sett.fTinyTextures)
    {
        sizes.push_back(eTileSize::s15);
        if (mTileSize == eTileSize::s15)
        {
            currSize = sizes.size() - 1;
        }
    }
    if (sett.fSmallTextures)
    {
        sizes.push_back(eTileSize::s30);
        if (mTileSize == eTileSize::s30)
        {
            currSize = sizes.size() - 1;
        }
    }
    if (sett.fMediumTextures)
    {
        sizes.push_back(eTileSize::s45);
        if (mTileSize == eTileSize::s45)
        {
            currSize = sizes.size() - 1;
        }
    }
    if (sett.fLargeTextures)
    {
        sizes.push_back(eTileSize::s60);
        if (mTileSize == eTileSize::s60)
        {
            currSize = sizes.size() - 1;
        }
    }
    const int sizesC = sizes.size();
    if (e.dy() > 0)
    {
        const int newSize = currSize + 1;
        if (newSize < sizesC)
        {
            setTileSize(sizes[newSize]);
        }
    }
    else
    {
        const int newSize = currSize - 1;
        if (newSize >= 0)
        {
            setTileSize(sizes[newSize]);
        }
    }
    return true;
}

void eGameWidget::renderTargetsReset()
{
    eWidget::renderTargetsReset();
    initializeNumbers();
}

void eGameWidget::showGoals()
{
    const auto w = window();
    const auto c = w->campaign();

    const auto e = new eEpisodeIntroductionWidget(w);
    const auto proceedA = [e]()
    {
        e->deleteLater();
    };
    e->resize(width(), height());
    const auto ee = c->currentEpisode();
    e->initialize(c,
                  c->titleText(),
                  ee->fIntroduction,
                  mBoard->goals(),
                  proceedA,
                  eEpisodeIntroType::goals);
    addWidget(e);
    e->align(eAlignment::vcenter);
    e->setX(x() + (width() - e->width() - mGm->width()) / 2);
    window()->execDialog(e);
}

void eGameWidget::showOptionsMenu()
{
    const auto d = new eOptionsMenu(getOptionsPages(window()), window());
    d->initialize();
    window()->execDialog(d, true, [this]()
                         { window()->setWidget(this); });
}

void eGameWidget::showGraphicsMenu()
{
    const auto w = window();
    const auto esm = new eGraphicsMenu(w->settings(), w);
    esm->resize(width(), height());
    const auto applyA = [this, esm, w](const eSettings &settings)
    {
        const bool loadNeeded = settings.fRes != w->settings().fRes;
        w->applyGraphicsSettings(settings);
        if (!loadNeeded)
        {
            removeWidget(esm);
            esm->deleteLater();
        }
    };
    const auto fullscreenA = [w](const bool f)
    {
        w->setFullscreen(f);
    };
    esm->initialize(applyA, fullscreenA);
    addWidget(esm);
    esm->align(eAlignment::center);
    w->execDialog(esm);
}

void eGameWidget::showStampManager()
{
    mGm->setMode(eBuildingMode::none);
    const auto ppid = mBoard->personPlayer();
    const auto diff = mBoard->difficulty(ppid);
    const auto d = new eStampManager(window());
    d->initialize(mStampTool.get(), diff);
    d->setTemplateSelectedAction([this]() {
        mGm->setMode(eBuildingMode::stamp);
    });
    d->setCreateTemplateAction([this]() {
        beginStampTemplateCreate();
    });
    window()->execDialog(d);
    d->align(eAlignment::center);
}

void eGameWidget::beginStampTemplateCreate()
{
    mGm->clearMode();
    mGm->closeBuildWidget();
    setPatrolBuilding(nullptr);
    eCursors::set(eCursorType::defaultCursor);

    mCreatingStampTemplate = true;
    mStampTemplateTiles.clear();

    if(mStampTemplatePanel) {
        mStampTemplatePanel->deleteLater();
        mStampTemplatePanel = nullptr;
        mStampTemplateStats = nullptr;
    }

    const auto panel = new eFramedWidget(window());
    panel->setType(eFrameType::message);
    const int p = panel->padding();

    mStampTemplateStats = new eLabel(window());
    mStampTemplateStats->setFontSizeS();
    mStampTemplateStats->setNoPadding();
    panel->addWidget(mStampTemplateStats);
    mStampTemplateStats->move(p, p);

    const auto saveB = new eFramedButton(window());
    saveB->setUnderline(false);
    saveB->setFontSizeS();
    saveB->setPaddingS();
    saveB->setText("Save");
    saveB->fitContent();
    saveB->setPressAction([this]() {
        showStampTemplateNameDialog();
    });
    panel->addWidget(saveB);

    const auto cancelB = new eFramedButton(window());
    cancelB->setUnderline(false);
    cancelB->setFontSizeS();
    cancelB->setPaddingS();
    cancelB->setText("Cancel");
    cancelB->fitContent();
    cancelB->setPressAction([this]() {
        cancelStampTemplateCreate();
    });
    panel->addWidget(cancelB);

    updateStampTemplatePanel();
    saveB->setY(mStampTemplateStats->y() + mStampTemplateStats->height() + p);
    cancelB->setY(saveB->y());
    saveB->setX(p);
    cancelB->setX(saveB->x() + saveB->width() + p);
    const int panelW = std::max(mStampTemplateStats->x() + mStampTemplateStats->width(),
                                cancelB->x() + cancelB->width()) + p;
    const int panelH = cancelB->y() + cancelB->height() + p;
    panel->resize(panelW, panelH);
    addWidget(panel);
    panel->align(eAlignment::bottom | eAlignment::hcenter);
    panel->setY(panel->y() - 2 * padding());
    mStampTemplatePanel = panel;
}

void eGameWidget::cancelStampTemplateCreate()
{
    mCreatingStampTemplate = false;
    mStampTemplateTiles.clear();
    if(mStampTemplatePanel) {
        mStampTemplatePanel->deleteLater();
        mStampTemplatePanel = nullptr;
        mStampTemplateStats = nullptr;
    }
    eCursors::set(eCursorType::defaultCursor);
}

void eGameWidget::updateStampTemplateSelection()
{
    mStampTemplateTiles.clear();
    if(!mBoard) return;

    const int minX = std::min(mPressedTX, mHoverTX);
    const int minY = std::min(mPressedTY, mHoverTY);
    const int maxX = std::max(mPressedTX, mHoverTX);
    const int maxY = std::max(mPressedTY, mHoverTY);

    std::set<eBuilding*> buildings;
    for(int x = minX; x <= maxX; x++) {
        for(int y = minY; y <= maxY; y++) {
            const auto tile = mBoard->tile(x, y);
            if(!tile) continue;
            auto b = tile->underBuilding();
            if(const auto space = dynamic_cast<eAgoraSpace*>(b)) {
                b = space->agora();
            }
            if(b) buildings.insert(b);
        }
    }

    std::set<eTile*> tiles;
    for(const auto b : buildings) {
        for(const auto tile : b->tilesUnder()) {
            if(tile && tiles.insert(tile).second) {
                mStampTemplateTiles.push_back(tile);
            }
        }
    }
    updateStampTemplatePanel();
}

void eGameWidget::updateStampTemplatePanel()
{
    if(!mStampTemplateStats) return;
    int buildings = 0;
    int roads = 0;
    stampTemplateElements(&buildings, &roads);
    mStampTemplateStats->setText("Buildings: " + std::to_string(buildings) +
                                 "  Roads: " + std::to_string(roads));
    mStampTemplateStats->fitContent();
    if(mStampTemplatePanel) {
        const int neededW = mStampTemplateStats->x() +
                            mStampTemplateStats->width() +
                            2 * mStampTemplatePanel->padding();
        if(neededW > mStampTemplatePanel->width()) {
            mStampTemplatePanel->resize(neededW, mStampTemplatePanel->height());
            mStampTemplatePanel->align(eAlignment::bottom | eAlignment::hcenter);
            mStampTemplatePanel->setY(mStampTemplatePanel->y() - 2 * padding());
        }
    }
}

void eGameWidget::showStampTemplateNameDialog()
{
    if(stampTemplateElements().empty()) return;
    mStampTemplateNameDialogOpen = true;

    const auto d = new eFramedWidget(window());
    d->setType(eFrameType::message);
    const int p = d->padding();

    const auto title = new eLabel("Template name", window());
    title->setFontSizeS();
    title->fitContent();
    d->addWidget(title);
    title->move(p, p);

    const auto edit = new eLineEdit(window());
    edit->setRenderBg(true);
    edit->setText("template");
    edit->fitContent();
    edit->setWidth(3 * edit->width() / 2);
    d->addWidget(edit);
    edit->move(p, title->y() + title->height() + p);

    const auto okB = new eFramedButton(window());
    okB->setUnderline(false);
    okB->setFontSizeS();
    okB->setPaddingS();
    okB->setText("OK");
    okB->fitContent();
    okB->setPressAction([this, d, edit]() {
        saveStampTemplate(edit->text());
        mStampTemplateNameDialogOpen = false;
        d->deleteLater();
    });
    d->addWidget(okB);

    const auto cancelB = new eFramedButton(window());
    cancelB->setUnderline(false);
    cancelB->setFontSizeS();
    cancelB->setPaddingS();
    cancelB->setText("Cancel");
    cancelB->fitContent();
    cancelB->setPressAction([this, d]() {
        mStampTemplateNameDialogOpen = false;
        d->deleteLater();
    });
    d->addWidget(cancelB);

    okB->move(p, edit->y() + edit->height() + p);
    cancelB->move(okB->x() + okB->width() + p, okB->y());
    const int w = std::max(edit->x() + edit->width(),
                           cancelB->x() + cancelB->width()) + p;
    const int h = cancelB->y() + cancelB->height() + p;
    d->resize(w, h);

    window()->execDialog(d);
    d->align(eAlignment::center);
    edit->grabKeyboard();
}

void eGameWidget::saveStampTemplate(const std::string& name)
{
    const auto elements = stampTemplateElements();
    if(elements.empty()) return;

    auto safeName = name;
    for(auto& c : safeName) {
        const auto uc = static_cast<unsigned char>(c);
        if(!std::isalnum(uc) && c != '-' && c != '_') c = '-';
    }
    while(!safeName.empty() && safeName.front() == '-') safeName.erase(safeName.begin());
    while(!safeName.empty() && safeName.back() == '-') safeName.pop_back();
    if(safeName.empty()) safeName = "template";

    namespace fs = std::filesystem;
    const fs::path dir = eGameDir::stampsDir();
    fs::path path = dir / (safeName + ".txt");
    int suffix = 2;
    while(fs::exists(path)) {
        path = dir / (safeName + "-" + std::to_string(suffix++) + ".txt");
    }

    const auto pathString = path.u8string();
    if(!eWriteStampTemplate(pathString, elements)) {
        printf("Failed to write stamp template: %s\n", pathString.c_str());
        return;
    }

    mStampTool->setTemplate(path.stem().u8string(), pathString);
    cancelStampTemplateCreate();
    mGm->setMode(eBuildingMode::stamp);
}

std::vector<eStampElement> eGameWidget::stampTemplateElements(
        int* const buildingCount,
        int* const roadCount) const
{
    if(buildingCount) *buildingCount = 0;
    if(roadCount) *roadCount = 0;

    std::set<eBuilding*> buildings;
    for(const auto tile : mStampTemplateTiles) {
        if(!tile) continue;
        auto b = tile->underBuilding();
        if(const auto space = dynamic_cast<eAgoraSpace*>(b)) {
            b = space->agora();
        }
        if(b) buildings.insert(b);
    }

    const auto anchor = [](const eBuilding* const b, int& x, int& y) {
        const auto& r = b->tileRect();
        x = r.x;
        y = r.y;
        const int sw = r.w;
        const int sh = r.h;
        if(sw == 2 && sh == 2) {
            y += 1;
        } else if(sw == 3 && sh == 3) {
            x += 1;
            y += 1;
        } else if(sw == 4 || sh == 4) {
            x += 1;
            y += 2;
        } else if(sw == 5 || sh == 5) {
            x += 2;
            y += 2;
        } else if(sw == 6 || sh == 6) {
            x += 2;
            y += 2;
        }
    };

    int minX = INT_MAX;
    int minY = INT_MAX;
    for(const auto b : buildings) {
        if(b->type() == eBuildingType::agoraSpace) continue;
        int x;
        int y;
        anchor(b, x, y);
        minX = std::min(minX, x);
        minY = std::min(minY, y);
    }
    if(minX == INT_MAX || minY == INT_MAX) return {};

    std::vector<eStampElement> result;
    for(const auto b : buildings) {
        const auto type = b->type();
        if(type == eBuildingType::agoraSpace) continue;

        int x;
        int y;
        anchor(b, x, y);
        int id = -1;
        if(type == eBuildingType::commonAgora) {
            if(const auto agora = dynamic_cast<const eAgoraBase*>(b)) {
                id = static_cast<int>(agora->orientation());
            }
        }

        if(type == eBuildingType::road || type == eBuildingType::roadblock) {
            if(roadCount) (*roadCount)++;
        } else {
            if(buildingCount) (*buildingCount)++;
        }
        result.push_back({type, x - minX, y - minY, id});
    }

    std::sort(result.begin(), result.end(),
              [](const eStampElement& a, const eStampElement& b) {
        const bool ar = a.type == eBuildingType::road ||
                        a.type == eBuildingType::roadblock;
        const bool br = b.type == eBuildingType::road ||
                        b.type == eBuildingType::roadblock;
        if(ar != br) return ar;
        if(a.dy != b.dy) return a.dy < b.dy;
        return a.dx < b.dx;
    });
    return result;
}

void eGameWidget::selectHoveredBuildingMode()
{
    if (hasInfoWidget() || mPatrolBuilding)
        return;
    const auto b = mBoard->buildingAt(mHoverTX, mHoverTY);
    if (!b)
        return;

    auto mode = eBuildingModeHelpers::fromBuildingType(b->type());
    if (mode == eBuildingMode::none)
    {
        if (const auto c = dynamic_cast<eCommemorative *>(b))
        {
            mode = static_cast<eBuildingMode>(
                static_cast<int>(eBuildingMode::populationMonument) + c->id());
        }
        else if (const auto g = dynamic_cast<eGodMonument *>(b))
        {
            mode = static_cast<eBuildingMode>(
                static_cast<int>(eBuildingMode::aphroditeMonument) +
                static_cast<int>(g->god()));
        }
        else if (const auto s = dynamic_cast<eAgoraSpace *>(b))
        {
            const auto agora = s->agora();
            if (agora)
            {
                mode = eBuildingModeHelpers::fromBuildingType(agora->type());
            }
        }
        else if (const auto gt = dynamic_cast<eGodMonumentTile *>(b))
        {
            const auto monument = gt->monument();
            if (monument)
            {
                mode = static_cast<eBuildingMode>(
                    static_cast<int>(eBuildingMode::aphroditeMonument) +
                    static_cast<int>(monument->god()));
            }
        }
    }

    if (mode == eBuildingMode::none)
        return;
    if (!eBuildingModeHelpers::isCloneable(mode))
        return;
    if (!mBoard->supportsBuilding(mViewedCityId, mode))
        return;
    mGm->setMode(mode);
}

void eGameWidget::setDX(const int dx)
{
    const int oldDX = mDX;
    mDX = dx;
    clampViewBox();
    updateMinimap();
    mPressedX += mDX - oldDX;
    mUpdateViewedTileScheduled = true;
}

void eGameWidget::setDY(const int dy)
{
    const int oldDY = mDY;
    mDY = dy;
    clampViewBox();
    updateMinimap();
    mPressedY += mDY - oldDY;
    mUpdateViewedTileScheduled = true;
}

void eGameWidget::clampViewBox()
{
    if (mTem->visible())
        return;
    const auto dir = mBoard->direction();
    const int w = mBoard->rotatedWidth();
    const int ww = width() - mGm->width();
    mDX = std::min(0, mDX);
    const int winc = dir == eWorldDirection::W ? mTileW / 2 : 0;
    mDX = std::max(-w * mTileW + ww + mTileW / 2 + winc, mDX);

    const int h = mBoard->rotatedHeight();
    const int hh = height();
    const int einc = dir == eWorldDirection::E ? mTileH / 2 : 0;
    const int dt = mTopMinAltitude < 0 ? mTopMinAltitude : 0;
    mDY = std::min(-mTileH / 2 + 2 * einc + dt * mTileH, mDY);
    const int db = mBottomMaxAltitude > 0 ? mBottomMaxAltitude : 0;
    mDY = std::max(-h * mTileH / 2 + hh + einc + db * mTileH, mDY);
}

void eGameWidget::setBookmark(const int id)
{
    const auto tile = mViewedTile;
    if (!tile)
    {
        mBookmarks.erase(id);
    }
    else
    {
        const int tx = tile->x();
        const int ty = tile->y();
        mBookmarks[id] = {tx, ty};
    }
}

void eGameWidget::viewBookmark(const int id)
{
    if (!mBoard)
        return;
    const auto it = mBookmarks.find(id);
    if (it == mBookmarks.end())
        return;
    const auto &c = it->second;
    const int tx = c.first;
    const int ty = c.second;
    const auto tile = mBoard->tile(tx, ty);
    if (!tile)
        return;
    viewTile(tile);
}

void eGameWidget::updateTopBottomAltitude()
{
    if (!mBoard)
        return;
    const auto dir = mBoard->direction();
    int i;
    if (dir == eWorldDirection::N)
    {
        mBoard->topElevationExtremas(mTopMinAltitude, i);
        mBoard->bottomElevationExtremas(i, mBottomMaxAltitude);
    }
    else if (dir == eWorldDirection::E)
    {
        mBoard->rightElevationExtremas(mTopMinAltitude, i);
        mBoard->leftElevationExtremas(i, mBottomMaxAltitude);
    }
    else if (dir == eWorldDirection::S)
    {
        mBoard->bottomElevationExtremas(mTopMinAltitude, i);
        mBoard->topElevationExtremas(i, mBottomMaxAltitude);
    }
    else
    { // if(dir == eWorldDirection::W) {
        mBoard->leftElevationExtremas(mTopMinAltitude, i);
        mBoard->rightElevationExtremas(i, mBottomMaxAltitude);
    }
}

void eGameWidget::updateMinMaxAltitude()
{
    mBoard->minMaxAltitude(mMinAltitude, mMaxAltitude);
}

void eGameWidget::updateMaps(const bool totalUpdate)
{
    const auto mm = mGm->miniMap();
    const auto mma = mAm->miniMap();
    const auto mmt = mTem->miniMap();
    const auto dir = mBoard->direction();
    mm->setDirection(dir);
    mma->setDirection(dir);
    mmt->setDirection(dir);
    const auto func = totalUpdate ? &eMiniMap::scheduleTotalUpdate : &eMiniMap::scheduleUpdate;
    (mm->*func)();
    (mma->*func)();
    (mmt->*func)();
}

void eGameWidget::updateMaps(const std::vector<eTile *> &tiles)
{
    const auto mm = mGm->miniMap();
    const auto mma = mAm->miniMap();
    const auto mmt = mTem->miniMap();
    mm->scheduleTilesUpdate(tiles);
    mma->scheduleTilesUpdate(tiles);
    mmt->scheduleTilesUpdate(tiles);
}

void eGameWidget::updateCitiesOnBoard()
{
    mTem->updateCitiesOnBoard(*mBoard);
}

void eGameWidget::setTileSize(const eTileSize size)
{
    const auto &setts = window()->settings();
    const auto sizes = setts.availableSizes();
    if (eVectorHelpers::contains(sizes, size))
    {
        mTileSize = size;
    }
    else
    {
        mTileSize = sizes[0];
    }
    const int tid = static_cast<int>(mTileSize);
    const auto &trrTexs = eGameTextures::terrain().at(tid);
    const int newW = trrTexs.fTileW;
    const int newH = trrTexs.fTileH;

    const double dnewW = newW;
    const double dnewH = newH;

    const int dx = std::round((mDX - width() / 2) * dnewW / mTileW + width() / 2);
    const int dy = std::round((mDY - height() / 2) * dnewH / mTileH + height() / 2);

    mTileW = newW;
    mTileH = newH;

    setDX(dx);
    setDY(dy);

    updateViewBoxSize();

    clampViewBox();

    updateTerrainTextures();
}

void eGameWidget::updateViewBoxSize()
{
    if (!mBoard || !mGm)
        return;
    double fx;
    double fy;
    viewBoxSize(fx, fy);
    const auto mm = mGm->miniMap();
    mm->setViewBoxSize(fx, fy);
    const auto mma = mAm->miniMap();
    mma->setViewBoxSize(fx, fy);
    const auto mmt = mTem->miniMap();
    mmt->setViewBoxSize(fx, fy);
}

void eGameWidget::setWorldDirection(const eWorldDirection dir)
{
    if (!mBoard)
        return;
    const auto tile = viewedTile();
    mBoard->setWorldDirection(dir);
    updateTopBottomAltitude();
    viewTile(tile);
    clampViewBox();
    updateMaps(true);
    updateViewBoxSize();
    mGm->setWorldDirection(dir);
    if (mTem)
        mTem->setWorldDirection(dir);
}

void eGameWidget::centerDialog(eWidget *const d)
{
    d->setY((height() - d->height() - mTopBar->height()) / 2);
    d->setX((width() - d->width() - mGm->width()) / 2);
}

void eGameWidget::openDialog(eWidget *const d)
{
    addWidget(d);
    centerDialog(d);
    window()->execDialog(d);
}

void eGameWidget::updateRequestButtons()
{
    mGm->updateRequestButtons();
}

void eGameWidget::setMessageListWidget(eMessageListWidget* const w)
{
    mMsgListWidget = w;
    if(mBoard) w->setBoard(mBoard);
}
