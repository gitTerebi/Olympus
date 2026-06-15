#include "game-menu.h"

#include <algorithm>

#include "textures/game-textures.h"
#include "main-window.h"
#include "echeckablebutton.h"
#include "engine/game-board.h"
#include "engine/difficulty.h"

#include "widgets/datawidgets/population-data-widget.h"
#include "widgets/datawidgets/eemploymentdatawidget.h"
#include "widgets/datawidgets/eappealdatawidget.h"
#include "widgets/datawidgets/storage-sidebar-panel.h"
#include "widgets/datawidgets/ehygienesafetydatawidget.h"
#include "widgets/datawidgets/eculturedatawidget.h"
#include "widgets/datawidgets/esciencedatawidget.h"
#include "widgets/datawidgets/eadmindatawidget.h"
#include "widgets/datawidgets/ehusbandrydatawidget.h"
#include "widgets/datawidgets/emythologydatawidget.h"
#include "widgets/datawidgets/emilitarydatawidget.h"
#include "widgets/datawidgets/overview-data-widget.h"
#include "eminimap.h"

#include "eeventwidget.h"

#include "game-widget.h"
#include "e-message-list-widget.h"

#include "language.h"
#include "engine/eresourcetype.h"

#include "ebuildwidget.h"
#include "elayouthelpers.h"
#include "ebasicbutton.h"
#include "erotatebutton.h"

struct eSubButtonData
{
    eBuildingMode fMode;
    std::string fName;
    std::function<void()> fPressedFunc;
    int fPrice;
    int fPriceSpace;
    const TextureCollection *fColl;
    const TextureCollection *fAColl;
    std::vector<eSPR> fSpr = {};
};

void tradePosts(const eCityId cid, std::vector<eSPR> &cs,
                GameBoard &board, const bool showAllPossibleBuildings)
{
    const auto pid = board.cityIdToPlayerId(cid);
    const auto ppid = board.personPlayer();
    if (pid != ppid && !showAllPossibleBuildings)
        return;
    const auto &wrld = board.world();
    int i = -1;
    for (const auto &c : wrld.cities())
    {
        const auto cCid = c->cityId();
        i++;
        if (c->isRival() && !showAllPossibleBuildings)
            continue;
        if (cid == cCid)
            continue;
        if (!c->active() && !showAllPossibleBuildings)
            continue;
        if (!c->visible() && !showAllPossibleBuildings)
            continue;
        if (board.hasTradePost(cid, *c))
            continue;
        const auto tradeCid = c->cityId();
        const auto tradePid = board.cityIdToPlayerId(tradeCid);
        const auto tradeC = board.boardCityWithId(tradeCid);
        const auto tradeTid = board.playerIdToTeamId(tradePid);
        const auto tid = board.playerIdToTeamId(pid);
        if (eTeamIdHelpers::isEnemy(tradeTid, tid))
            continue;
        if (!c->buys().empty() || !c->sells().empty() ||
            (tradeC && pid == tradePid))
        {
            if (c->waterTrade(cid))
            {
                const auto name = Language::zeusText(28, 60) + " " + c->name();
                const eSPR s{eBuildingMode::pier, name, 0, i};
                cs.push_back(s);
            }
            else
            {
                const auto name = Language::zeusText(28, 62) + " " + c->name();
                const eSPR s{eBuildingMode::tradePost, name, 0, i};
                cs.push_back(s);
            }
        }
    }
}

class eSubButton
{
public:
    eSubButton(const eBuildingMode mode,
               eButton *const button,
               eButton *const abutton,
               const std::vector<eSPR> &children,
               GameBoard &board) : mMode(mode),
                                    mButton(button),
                                    mAButton(abutton),
                                    mChildren(children),
                                    mBoard(board) {}

    void updateVisible(const eCityId cid, const bool showAllPossibleBuildings)
    {
        bool vis = false;
        const auto pid = mBoard.cityIdToPlayerId(cid);
        const auto ppid = mBoard.personPlayer();
        const bool a = mBoard.atlantean(cid);
        if (pid != ppid)
        {
            vis = showAllPossibleBuildings;
        }
        else if (mMode == eBuildingMode::tradePost)
        {
            std::vector<eSPR> cs;
            tradePosts(cid, cs, mBoard, showAllPossibleBuildings);
            vis = !cs.empty();
        }
        else if (mMode == eBuildingMode::palace)
        {
            vis = !mBoard.hasPalace(cid);
        }
        else if (mMode == eBuildingMode::stadium)
        {
            vis = !mBoard.atlantean(cid) && !mBoard.hasStadium(cid);
        }
        else if (mMode == eBuildingMode::museum)
        {
            vis = mBoard.atlantean(cid) && !mBoard.hasMuseum(cid);
        }
        else if (mMode == eBuildingMode::none)
        {
            for (const auto &c : mChildren)
            {
                const bool s = showAllPossibleBuildings ||
                               mBoard.supportsBuilding(cid, c.fMode);
                if (s)
                {
                    vis = true;
                    break;
                }
            }
        }
        else
        {
            vis = showAllPossibleBuildings ||
                  mBoard.supportsBuilding(cid, mMode);
        }
        if (mButton)
        {
            mButton->setVisible(vis && (!a || !mAButton));
        }
        if (mAButton)
        {
            mAButton->setVisible(vis && (a || !mButton));
        }
    }

private:
    const eBuildingMode mMode;
    eButton *const mButton;
    eButton *const mAButton;
    const std::vector<eSPR> mChildren;
    GameBoard &mBoard;
};

eWidget *GameMenu::createPriceWidget(const InterfaceTextures &coll)
{
    const auto r = new eWidget(window());
    r->setNoPadding();
    const auto plabel = new eLabel("0", window());
    plabel->setFontSizeXS();
    plabel->setPaddingXS();
    plabel->fitContent();
    const auto ilabel = new eScaledTextureLabel(window());
    ilabel->setTexture(coll.fDrachmasUnit);
    ilabel->setPaddingXS();
    ilabel->setMaxDrawHeight(plabel->height());
    ilabel->setFitToDrawSize(true);
    const auto row = eLayoutHelpers::createFlexContainer(
        window(), ilabel->width() + plabel->width(), 0,
        eLayoutHelpers::eFlexDirection::row,
        {{ilabel}, {plabel}},
        {.align = eLayoutHelpers::eAlign::center});
    r->addWidget(row);
    r->fitContent();
    mPriceWidgets.push_back(r);
    mPriceLabels.push_back(plabel);
    r->hide();
    return r;
}

eWidget *GameMenu::createSubButtons(
    const int resoltuionMult,
    const eButtonsDataVec &buttons)
{
    const auto result = new eWidget(window());

    const int x = resoltuionMult * 35;
    const int y = resoltuionMult * 28;
    const std::vector<std::pair<int, int>> poses =
        {{0, 0}, {x, 0}, {0, y}, {x, y}};

    const int iMax = buttons.size();
    for (int i = 0; i < iMax; i++)
    {
        const auto &c = buttons[i];

        const auto createButton = [&](const TextureCollection &texs)
        {
            const auto b = eButton::sCreate(texs, window(), result);
            b->setPressAction(c.fPressedFunc);
            b->setMouseEnterAction([c, this]()
                                   {
                mNameLabel->setText(c.fName);
                displayPrice(c.fPrice, c.fPriceSpace); });
            b->setMouseLeaveAction([c, this]()
                                   {
                mNameLabel->setText("");
                displayPrice(0, c.fPriceSpace); });
            const auto &pos = poses[i];
            b->setX(pos.first);
            b->setY(pos.second);

            return b;
        };

        const auto b = c.fColl ? createButton(*c.fColl) : nullptr;
        const auto ab = c.fAColl ? createButton(*c.fAColl) : nullptr;

        const auto subButton = new eSubButton(c.fMode, b, ab, c.fSpr, *mBoard);
        subButton->updateVisible(eCityId::neutralFriendly,
                                 mShowAllPossibleBuildings);
        mSubButtons.push_back(subButton);
    }

    result->setNoPadding();
    result->fitContent();

    return result;
}

eBuildButton *GameMenu::createBuildButton(const eSPR &c)
{
    const auto bb = new eBuildButton(window());
    const auto pid = mBoard->personPlayer();
    const auto diff = mBoard->difficulty(pid);
    const auto mode = c.fMode;
    const auto t = eBuildingModeHelpers::toBuildingType(mode);
    const int cost = DifficultyHelpers::buildingCost(diff, t);
    const int mult = static_cast<int>(resolution().uiScale()) + 1;
    bb->initialize(c.fName, c.fMarbleCost, cost, 150 * mult);
    bb->setPressAction([this, c]()
                       {
        setMode(c.fMode);
        mTradeCityId = c.fCity;
        closeBuildWidget(); });
    return bb;
}

eBuildButton *GameMenu::createTradeBuildButton(
    const eSPR &c, const int tradeIconWidth)
{
    const auto mode = c.fMode;
    const bool isTrade = mode == eBuildingMode::tradePost || mode == eBuildingMode::pier;
    if (!isTrade)
        return createBuildButton(c);
    const auto bb = new eBuildButton(window());
    const auto pid = mBoard->personPlayer();
    const auto diff = mBoard->difficulty(pid);
    const auto t = eBuildingModeHelpers::toBuildingType(mode);
    const int cost = DifficultyHelpers::buildingCost(diff, t);
    bb->initialize(c.fName, 0, 0);
    int x = 10;
    {
        const auto &wrld = mBoard->world();
        const auto &cities = wrld.cities();
        if (c.fCity >= 0 && c.fCity < cities.size())
        {
            const auto city = cities[c.fCity];
            const int mult = bb->height() / 10;
            const int maxTradeWidth = 250 * mult;
            const int defaultGap = 2 * mult;
            int gap = defaultGap;
            const int endPadding = 10 * mult;
            auto scale = resolution().uiScale();
            x += 100 * mult;
            // Buys label
            const auto buysLabel = new eLabel("Buys:", window());
            buysLabel->setFontSizeS();
            buysLabel->setNoPadding();
            buysLabel->fitContent();
            // Sells label
            const auto sellsLabel = new eLabel("Sells:", window());
            sellsLabel->setFontSizeS();
            sellsLabel->setNoPadding();
            sellsLabel->fitContent();

            const auto tradeWidth = [&](const eUIScale iconScale,
                                        const int iconGap)
            {
                int result = x;
                result += buysLabel->width() + iconGap;
                int iconWidth = 0;
                for (const auto &buy : city->buys())
                {
                    const auto tex = eResourceTypeHelpers::icon(iconScale, buy.fType);
                    if (tex)
                        iconWidth = std::max(iconWidth, tex->width());
                }
                for (const auto &sell : city->sells())
                {
                    const auto tex = eResourceTypeHelpers::icon(iconScale, sell.fType);
                    if (tex)
                        iconWidth = std::max(iconWidth, tex->width());
                }
                result += static_cast<int>(city->buys().size()) * (iconWidth + iconGap);
                result += sellsLabel->width() + iconGap;
                result += static_cast<int>(city->sells().size()) * (iconWidth + iconGap);
                return result + endPadding;
            };

            const auto iconSlotWidth = [&]()
            {
                int result = 0;
                for (const auto &buy : city->buys())
                {
                    const auto tex = eResourceTypeHelpers::icon(scale, buy.fType);
                    if (tex)
                        result = std::max(result, tex->width());
                }
                for (const auto &sell : city->sells())
                {
                    const auto tex = eResourceTypeHelpers::icon(scale, sell.fType);
                    if (tex)
                        result = std::max(result, tex->width());
                }
                return result;
            };

            if (tradeWidth(scale, gap) > maxTradeWidth)
            {
                scale = eUIScale::tiny;
                gap = std::max(1, mult / 2);
            }

            bb->setWidth(maxTradeWidth);
            const int iconWidth = std::max(
                tradeIconWidth > 0 ? tradeIconWidth : iconSlotWidth(),
                8 * mult);
            const int buyLabelX = 105 * mult;
            const int tradeSlots = 2;
            const int buyIconsX = buyLabelX + buysLabel->width() + gap;
            const int sellLabelX = buyIconsX + tradeSlots * (iconWidth + gap);
            const int sellIconsX = sellLabelX + sellsLabel->width() + gap;
            const int costX = maxTradeWidth - 35 * mult;
            const int iconDrawHeight = std::max(1, 10 * mult);
            const auto createTradeIcon = [&](const eResourceType type)
            {
                const auto icon = new eScaledTextureLabel(window());
                icon->setTexture(eResourceTypeHelpers::icon(scale, type));
                icon->setNoPadding();
                icon->fitContent();
                icon->setMaxDrawHeight(iconDrawHeight);
                icon->resize(iconWidth, bb->height());
                return icon;
            };

            bb->addWidget(buysLabel);
            buysLabel->align(Alignment::vcenter);
            buysLabel->setX(buyLabelX);
            x = buyIconsX;
            // Buy icons
            for (const auto &buy : city->buys())
            {
                const auto icon = createTradeIcon(buy.fType);
                bb->addWidget(icon);
                icon->align(Alignment::vcenter);
                icon->setX(x);
                x += iconWidth + gap;
            }
            bb->addWidget(sellsLabel);
            sellsLabel->align(Alignment::vcenter);
            sellsLabel->setX(sellLabelX);
            x = sellIconsX;
            // Sell icons
            for (const auto &sell : city->sells())
            {
                const auto icon = createTradeIcon(sell.fType);
                bb->addWidget(icon);
                icon->align(Alignment::vcenter);
                icon->setX(x);
                x += iconWidth + gap;
            }
            x = costX;
        }
    }

    // Add costs
    if (c.fMarbleCost > 0 || cost > 0)
    {
        const int mult = bb->height() / 10;
        const int iRes = mult - 1;
        const auto &intrfc = GameTextures::interface();
        const auto &coll = intrfc[iRes];
        if (c.fMarbleCost > 0)
        {
            const auto marbleIcon = new eLabel(window());
            marbleIcon->setTexture(coll.fMarbleUnit);
            marbleIcon->setNoPadding();
            marbleIcon->fitContent();
            const auto cstr = std::to_string(c.fMarbleCost);
            const auto marbleText = new eLabel(cstr, window());
            marbleText->setFontSizeS();
            marbleText->setNoPadding();
            marbleText->fitContent();
            bb->addWidget(marbleIcon);
            bb->addWidget(marbleText);
            marbleIcon->align(Alignment::vcenter);
            marbleText->align(Alignment::vcenter);
            marbleIcon->setX(x);
            marbleText->setX(marbleIcon->x() + marbleIcon->width());
            x += marbleText->x() + marbleText->width() - x + 10;
        }
        if (cost > 0)
        {
            const auto drachmaIcon = new eLabel(window());
            drachmaIcon->setTexture(coll.fDrachmasUnit);
            drachmaIcon->setNoPadding();
            drachmaIcon->fitContent();
            const auto cstr = std::to_string(cost);
            const auto drachmaText = new eLabel(cstr, window());
            drachmaText->setFontSizeS();
            drachmaText->setNoPadding();
            drachmaText->fitContent();
            bb->addWidget(drachmaIcon);
            bb->addWidget(drachmaText);
            drachmaIcon->align(Alignment::vcenter);
            drachmaText->align(Alignment::vcenter);
            drachmaIcon->setX(x);
            drachmaText->setX(drachmaIcon->x() + drachmaIcon->width());
            x = drachmaText->x() + drachmaText->width() + 10 * mult;
        }
    }
    bb->setPressAction([this, c]()
                       {
        setMode(c.fMode);
        mTradeCityId = c.fCity;
        closeBuildWidget(); });
    return bb;
}

void GameMenu::openBuildWidget(const int cmx, const int cmy,
                                const std::vector<eSPR> &cs)
{
    const auto cid = mGW->viewedCity();
    const auto pid = mBoard->cityIdToPlayerId(cid);
    const auto ppid = mBoard->personPlayer();
    if (pid != ppid && !mShowAllPossibleBuildings)
        return;
    std::vector<eBuildButton *> ws;
    int tradeIconWidth = 0;
    const auto scale = resolution().uiScale();
    const auto &cities = mBoard->world().cities();
    for (const auto &c : cs)
    {
        const auto mode = c.fMode;
        const bool isTrade = mode == eBuildingMode::tradePost ||
                             mode == eBuildingMode::pier;
        if (!isTrade || c.fCity < 0 || c.fCity >= cities.size())
            continue;
        const auto city = cities[c.fCity];
        for (const auto &buy : city->buys())
        {
            const auto tex = eResourceTypeHelpers::icon(scale, buy.fType);
            if (tex)
                tradeIconWidth = std::max(tradeIconWidth, tex->width());
        }
        for (const auto &sell : city->sells())
        {
            const auto tex = eResourceTypeHelpers::icon(scale, sell.fType);
            if (tex)
                tradeIconWidth = std::max(tradeIconWidth, tex->width());
        }
    }
    for (const auto &c : cs)
    {
        if (!mBoard->supportsBuilding(cid, c.fMode) &&
            !mShowAllPossibleBuildings)
            continue;
        const auto mode = c.fMode;
        const bool isTrade = mode == eBuildingMode::tradePost ||
                             mode == eBuildingMode::pier;
        const auto bb = isTrade ? createTradeBuildButton(c, tradeIconWidth) : createBuildButton(c);
        ws.push_back(bb);
    }
    if (ws.empty())
        return;
    const auto bw = new eBuildWidget(window());
    bw->initialize(ws);
    bw->exec(cmx - bw->width(), cmy - bw->height(), this);
    setBuildWidget(bw);
}

void GameMenu::setModeChangedAction(const eAction &func)
{
    mModeChangeAct = func;
}

void GameMenu::updateRequestButtons()
{
    mOverDataW->updateRequestButtons();
}

void GameMenu::setWorldDirection(const eWorldDirection dir)
{
    mRotateButton->setDirection(dir);
}

void GameMenu::update()
{
    if (mUndoButton && mBoard)
    {
        mUndoButton->setEnabled(mBoard->undoValid());
    }
}

void GameMenu::setShowAllPossibleBuildings(const bool b)
{
    mShowAllPossibleBuildings = b;
    updateButtonsVisibility();
}

void GameMenu::displayPrice(const int price, const int loc)
{
    const auto w = mPriceWidgets[loc];
    const auto l = mPriceLabels[loc];
    if (price <= 0)
    {
        w->hide();
    }
    else
    {
        l->setText(std::to_string(price));
        w->show();
    }
}

GameMenu::~GameMenu()
{
    for (const auto s : mSubButtons)
    {
        delete s;
    }
}

void GameMenu::initialize(GameBoard *const b,
                           const eAction &goalsView)
{
    mBoard = b;
    GameMenuBase::initialize();

    int iRes;
    int mult;
    iResAndMult(iRes, mult);

    const auto &intrfc = GameTextures::interface();
    const auto &coll = intrfc[iRes];
    const auto tex = coll.fGameMenuBackground;
    setTexture(tex);
    setPadding(0);
    fitContent();

    const int cmx = -padding();
    const int cmy = 5 * height() / 8;

    const int dataWidWidth = 65 * mult;
    const int dataWidHeight = 119 * mult;

    const int wwHeight = 190 * mult;
    const int wy = dataWidHeight + 31 * mult;
    const int wx = 24 * mult;

    const auto createDataWidgetBase =
        [&](eDataWidget *const dataW,
            eWidget *const w9,
            const std::string &name)
    {
        const auto ww9 = new eWidget(window());
        const auto alabel = new eLabel(window());
        alabel->setFontSizeS();
        alabel->setPaddingXS();
        alabel->setText(name);
        alabel->fitContent();
        ww9->addWidget(alabel);
        dataW->setWidth(dataWidWidth);
        dataW->setHeight(dataWidHeight);
        dataW->initialize();
        ww9->addWidget(dataW);
        ww9->setWidth(dataWidWidth);
        ww9->setHeight(wwHeight);
        ww9->stackVertically();
        alabel->align(Alignment::hcenter);
        ww9->addWidget(w9);
        w9->setY(wy);
        ww9->fitContent();
        return ww9;
    };

    const auto createDataWidget =
        [&](eDataWidget *const dataW,
            const eButtonsDataVec &buttonsVec,
            const std::string &name)
    {
        const auto w9 = createSubButtons(mult, buttonsVec);
        return createDataWidgetBase(dataW, w9, name);
    };

    mNameLabel = new eFramedLabel(window());
    mNameLabel->setType(eFrameType::inner);
    mNameLabel->setFontSizeXS();
    mNameLabel->setText("Recreational Areas");
    mNameLabel->setPaddingS();
    mNameLabel->fitContent();
    mNameLabel->setText("");
    mNameLabel->setWidth(dataWidWidth);
    mNameLabel->setX(wx);
    mNameLabel->setY(wy);
    addWidget(mNameLabel);

    {
        const auto ww = new eWidget(window());
        ww->setNoPadding();
        const int x = mult * 35;
        const int y = mult * 28;
        const std::vector<std::pair<int, int>> poses =
            {{0, 0}, {x, 0}, {0, y}, {x, y}};
        for (const auto &p : poses)
        {
            const auto w = createPriceWidget(coll);
            ww->addWidget(w);
            w->setX(p.first);
            w->setY(p.second);
        }
        ww->fitContent();
        addWidget(ww);
        ww->setX(wx);
        ww->setY(wy + 32 * mult);
    }

    mPopDataW = new PopulationDataWidget(*b, window());

    const auto pid = mBoard->personPlayer();
    const auto diff = mBoard->difficulty(pid);
    const int cost1 = DifficultyHelpers::buildingCost(
        diff, eBuildingType::commonHouse);
    const int cost2 = DifficultyHelpers::buildingCost(
        diff, eBuildingType::eliteHousing);

    const auto cha0 = [this]()
    {
        setMode(eBuildingMode::commonHousing);
    };
    const auto eha0 = [this]()
    {
        setMode(eBuildingMode::eliteHousing);
    };

    const auto buttonsVec0 = eButtonsDataVec{
        {eBuildingMode::commonHousing,
         Language::zeusText(28, 2),
         cha0, cost1, 0,
         &coll.fCommonHousing,
         &coll.fPoseidonCommonHousing},
        {eBuildingMode::eliteHousing,
         Language::zeusText(28, 9),
         eha0, cost2, 1,
         &coll.fEliteHousing,
         &coll.fPoseidonEliteHousing}};
    const auto ww0 = createDataWidget(mPopDataW, buttonsVec0,
                                      Language::zeusText(88, 0));

    const std::vector<eSPR> ff1spr = {eSPR{eBuildingMode::wheatFarm, Language::zeusText(28, 31)},
                                      eSPR{eBuildingMode::carrotFarm, Language::zeusText(28, 33)},
                                      eSPR{eBuildingMode::onionFarm, Language::zeusText(28, 32)}};

    const auto ff1 = [this, cmx, cmy, ff1spr]()
    {
        openBuildWidget(cmx, cmy, ff1spr);
    };

    const std::vector<eSPR> of1spr = {eSPR{eBuildingMode::vine, Language::zeusText(28, 35)},
                                      eSPR{eBuildingMode::oliveTree, Language::zeusText(28, 36)},
                                      eSPR{eBuildingMode::orangeTree, Language::zeusText(28, 217)},
                                      eSPR{eBuildingMode::orangeTendersLodge, Language::zeusText(28, 214)},
                                      eSPR{eBuildingMode::growersLodge, Language::zeusText(28, 37)}};

    const auto of1 = [this, cmx, cmy, of1spr]()
    {
        openBuildWidget(cmx, cmy, of1spr);
    };

    const std::vector<eSPR> af1spr = {eSPR{eBuildingMode::dairy, Language::zeusText(28, 42)},
                                      eSPR{eBuildingMode::goat, Language::zeusText(28, 39)},
                                      eSPR{eBuildingMode::cardingShed, Language::zeusText(28, 41)},
                                      eSPR{eBuildingMode::sheep, Language::zeusText(28, 40)}};
    const auto af1 = [this, cmx, cmy, af1spr]()
    {
        openBuildWidget(cmx, cmy, af1spr);
    };

    const std::vector<eSPR> ah1spr = {eSPR{eBuildingMode::fishery, Language::zeusText(28, 44)},
                                      eSPR{eBuildingMode::urchinQuay, Language::zeusText(28, 45)},
                                      eSPR{eBuildingMode::huntingLodge, Language::zeusText(28, 46)},
                                      eSPR{eBuildingMode::corral, Language::zeusText(28, 216)},
                                      eSPR{eBuildingMode::cattle, Language::zeusText(28, 220)}};

    const auto ah1 = [this, cmx, cmy, ah1spr]()
    {
        openBuildWidget(cmx, cmy, ah1spr);
    };

    mHusbDataW = new eHusbandryDataWidget(*b, window());
    const auto buttonsVec1 = eButtonsDataVec{
        {eBuildingMode::none,
         Language::zeusText(28, 30),
         ff1, 0, 0, &coll.fFoodFarming, nullptr, ff1spr},
        {eBuildingMode::none,
         Language::zeusText(28, 34),
         of1, 0, 1, &coll.fOtherFarming, nullptr, of1spr},
        {eBuildingMode::none,
         Language::zeusText(28, 38),
         af1, 0, 2, &coll.fAnimalFarming, nullptr, af1spr},
        {eBuildingMode::none,
         Language::zeusText(28, 43),
         ah1, 0, 3, &coll.fAnimalHunting, nullptr, ah1spr}};
    const auto ww1 = createDataWidget(mHusbDataW, buttonsVec1,
                                      Language::zeusText(88, 1));

    const std::vector<eSPR> r2spr = {eSPR{eBuildingMode::mint, Language::zeusText(28, 48)},
                                     eSPR{eBuildingMode::foundry, Language::zeusText(28, 50)},
                                     eSPR{eBuildingMode::timberMill, Language::zeusText(28, 51)},
                                     eSPR{eBuildingMode::masonryShop, Language::zeusText(28, 49)},
                                     eSPR{eBuildingMode::refinery, Language::zeusText(28, 211)},
                                     eSPR{eBuildingMode::blackMarbleWorkshop, Language::zeusText(28, 218)}};
    const auto r2 = [this, cmx, cmy, r2spr]()
    {
        openBuildWidget(cmx, cmy, r2spr);
    };
    const std::vector<eSPR> p2spr = {eSPR{eBuildingMode::winery, Language::zeusText(28, 53)},
                                     eSPR{eBuildingMode::olivePress, Language::zeusText(28, 54)},
                                     eSPR{eBuildingMode::sculptureStudio, Language::zeusText(28, 55)}};
    const auto p2 = [this, cmx, cmy, p2spr]()
    {
        openBuildWidget(cmx, cmy, p2spr);
    };
    const auto bg2 = [this]()
    {
        setMode(eBuildingMode::artisansGuild);
    };

    mEmplDataW = new eEmploymentDataWidget(*b, window());
    const int cost3 = DifficultyHelpers::buildingCost(
        diff, eBuildingType::artisansGuild);
    const auto buttonsVec2 = eButtonsDataVec{
        {eBuildingMode::none,
         Language::zeusText(28, 47),
         r2, 0, 0, &coll.fResources, nullptr, r2spr},
        {eBuildingMode::none,
         Language::zeusText(28, 52),
         p2, 0, 1, &coll.fProcessing, nullptr, p2spr},
        {eBuildingMode::artisansGuild,
         Language::zeusText(28, 56),
         bg2, cost3, 2, &coll.fArtisansGuild, nullptr}};
    const auto ww2 = createDataWidget(mEmplDataW, buttonsVec2,
                                      Language::zeusText(88, 2));

    const auto g3 = [this]()
    {
        setMode(eBuildingMode::granary);
    };
    const auto ww3 = [this]()
    {
        setMode(eBuildingMode::warehouse);
    };
    const std::vector<eSPR> a3spr = {eSPR{eBuildingMode::commonAgora, Language::zeusText(28, 63)},
                                     eSPR{eBuildingMode::grandAgora, Language::zeusText(28, 64)},
                                     eSPR{eBuildingMode::foodVendor, Language::zeusText(28, 68)},
                                     eSPR{eBuildingMode::fleeceVendor, Language::zeusText(28, 69)},
                                     eSPR{eBuildingMode::oilVendor, Language::zeusText(28, 70)},
                                     eSPR{eBuildingMode::wineVendor, Language::zeusText(28, 71)},
                                     eSPR{eBuildingMode::armsVendor, Language::zeusText(28, 72)},
                                     eSPR{eBuildingMode::horseTrainer, Language::zeusText(28, 73)},
                                     eSPR{eBuildingMode::chariotVendor, Language::zeusText(28, 215)}};
    const auto a3 = [this, cmx, cmy, a3spr]()
    {
        openBuildWidget(cmx, cmy, a3spr);
    };
    const auto t3 = [this, cmx, cmy]()
    {
        std::vector<eSPR> cs;
        const auto cid = mGW->viewedCity();
        tradePosts(cid, cs, *mBoard, mShowAllPossibleBuildings);
        openBuildWidget(cmx, cmy, cs);
    };

    mStrgDataW = new StorageSidebarPanel(*b, window());

    const int cost4 = DifficultyHelpers::buildingCost(
        diff, eBuildingType::granary);
    const int cost5 = DifficultyHelpers::buildingCost(
        diff, eBuildingType::warehouse);
    const auto buttonsVec3 = eButtonsDataVec{
        {eBuildingMode::granary,
         Language::zeusText(28, 57),
         g3, cost4, 0, &coll.fGranary, nullptr},
        {eBuildingMode::warehouse,
         Language::zeusText(28, 58),
         ww3, cost5, 1, &coll.fWarehouse, nullptr},
        {eBuildingMode::none,
         Language::zeusText(28, 67),
         a3, 0, 2, &coll.fAgoras, nullptr, a3spr},
        {eBuildingMode::tradePost,
         Language::zeusText(28, 26),
         t3, 0, 3, &coll.fTrade, nullptr}};
    const auto www3 = createDataWidget(mStrgDataW, buttonsVec3,
                                       Language::zeusText(88, 3));

    const auto ff4 = [this]()
    {
        setMode(eBuildingMode::maintenanceOffice);
    };
    const auto f4 = [this]()
    {
        setMode(eBuildingMode::fountain);
    };
    const auto p4 = [this]()
    {
        setMode(eBuildingMode::watchpost);
    };
    const auto h4 = [this]()
    {
        setMode(eBuildingMode::hospital);
    };

    mHySaDataW = new eHygieneSafetyDataWidget(*b, window());
    const int cost6 = DifficultyHelpers::buildingCost(
        diff, eBuildingType::fountain);
    const int cost7 = DifficultyHelpers::buildingCost(
        diff, eBuildingType::hospital);
    const int cost8 = DifficultyHelpers::buildingCost(
        diff, eBuildingType::maintenanceOffice);
    const int cost9 = DifficultyHelpers::buildingCost(
        diff, eBuildingType::watchPost);
    const auto buttonsVec4 = eButtonsDataVec{
        {eBuildingMode::fountain,
         Language::zeusText(28, 74),
         f4, cost6, 0, &coll.fFountain, nullptr},
        {eBuildingMode::hospital,
         Language::zeusText(28, 76),
         h4, cost7, 1, &coll.fHospital, nullptr},
        {eBuildingMode::maintenanceOffice,
         Language::zeusText(28, 121),
         ff4, cost8, 2, &coll.fFireFighter, nullptr},
        {eBuildingMode::watchpost,
         Language::zeusText(28, 124),
         p4, cost9, 3, &coll.fPolice, nullptr}};
    const auto ww4 = createDataWidget(mHySaDataW, buttonsVec4,
                                      Language::zeusText(88, 4));

    const auto p5 = [this]()
    {
        setMode(eBuildingMode::palace);
    };
    const auto tc5 = [this]()
    {
        setMode(eBuildingMode::taxOffice);
    };
    const auto bb5 = [this]()
    {
        setMode(eBuildingMode::bridge);
    };

    mAdminDataW = new eAdminDataWidget(*b, window());
    const int cost10 = DifficultyHelpers::buildingCost(
        diff, eBuildingType::palace);
    const int cost11 = DifficultyHelpers::buildingCost(
        diff, eBuildingType::taxOffice);
    const int cost12 = DifficultyHelpers::buildingCost(
        diff, eBuildingType::bridge);
    const std::vector<eSPR> d5spr = {eSPR{eBuildingMode::hippodromePiece, Language::zeusText(28, 200)},
                                     eSPR{eBuildingMode::crosswalk, Language::zeusText(28, 201)}};
    const auto d5 = [this, cmx, cmy, d5spr]()
    {
        openBuildWidget(cmx, cmy, d5spr);
    };
    const auto buttonsVec5 = eButtonsDataVec{
        {eBuildingMode::palace,
         Language::zeusText(28, 117),
         p5, cost10, 0, &coll.fPalace, nullptr},
        {eBuildingMode::taxOffice,
         Language::zeusText(28, 122),
         tc5, cost11, 1, &coll.fTaxCollector, nullptr},
        {eBuildingMode::bridge,
         Language::zeusText(28, 120),
         bb5, cost12, 2,
         &coll.fBridge,
         &coll.fPoseidonBridge},
        {eBuildingMode::none,
         Language::zeusText(28, 199),
         d5, 0, 3, &coll.fHipodrome, &coll.fHipodrome, d5spr}};
    const auto ww5 = createDataWidget(mAdminDataW, buttonsVec5,
                                      Language::zeusText(88, 5));

    eWidget *ww6 = nullptr;
    eWidget *ww7 = nullptr;
    {
        const std::vector<eSPR> p6spr = {eSPR{eBuildingMode::observatory, Language::zeusText(28, 203)},
                                         eSPR{eBuildingMode::university, Language::zeusText(28, 204)}};
        const auto p6 = [this, cmx, cmy, p6spr]()
        {
            openBuildWidget(cmx, cmy, p6spr);
        };
        const auto g6 = [this]()
        {
            setMode(eBuildingMode::bibliotheke);
        };
        const std::vector<eSPR> d6spr = {eSPR{eBuildingMode::laboratory, Language::zeusText(28, 205)},
                                         eSPR{eBuildingMode::inventorsWorkshop, Language::zeusText(28, 206)}};
        const auto d6 = [this, cmx, cmy, d6spr]()
        {
            openBuildWidget(cmx, cmy, d6spr);
        };
        const auto s6 = [this]()
        {
            setMode(eBuildingMode::museum);
        };
        mScienceDataW = new eScienceDataWidget(*mBoard, window());
        const int cost13 = DifficultyHelpers::buildingCost(
            diff, eBuildingType::bibliotheke);
        const int cost14 = DifficultyHelpers::buildingCost(
            diff, eBuildingType::museum);
        const auto buttonsVec6 = eButtonsDataVec{
            {eBuildingMode::bibliotheke,
             Language::zeusText(28, 202),
             g6, cost13, 0, nullptr, &coll.fBibliotheke},
            {eBuildingMode::none,
             Language::zeusText(28, 208),
             p6, 0, 1, nullptr, &coll.fAstronomy, p6spr},
            {eBuildingMode::none,
             Language::zeusText(28, 209),
             d6, 0, 2, nullptr, &coll.fTechnology, d6spr},
            {eBuildingMode::museum,
             Language::zeusText(28, 207),
             s6, cost14, 3, nullptr, &coll.fMuseum}};
        ww7 = createDataWidget(mScienceDataW, buttonsVec6,
                               Language::zeusText(88, 24));
    }
    {
        const std::vector<eSPR> p6spr = {eSPR{eBuildingMode::podium, Language::zeusText(28, 81)},
                                         eSPR{eBuildingMode::college, Language::zeusText(28, 77)}};
        const auto p6 = [this, cmx, cmy, p6spr]()
        {
            openBuildWidget(cmx, cmy, p6spr);
        };
        const auto g6 = [this]()
        {
            setMode(eBuildingMode::gymnasium);
        };
        const std::vector<eSPR> d6spr = {eSPR{eBuildingMode::theater, Language::zeusText(28, 82)},
                                         eSPR{eBuildingMode::dramaSchool, Language::zeusText(28, 78)}};
        const auto d6 = [this, cmx, cmy, d6spr]()
        {
            openBuildWidget(cmx, cmy, d6spr);
        };
        const auto s6 = [this]()
        {
            setMode(eBuildingMode::stadium);
        };

        mCultureDataW = new eCultureDataWidget(*mBoard, window());
        const int cost13 = DifficultyHelpers::buildingCost(
            diff, eBuildingType::gymnasium);
        const int cost14 = DifficultyHelpers::buildingCost(
            diff, eBuildingType::stadium);
        const auto buttonsVec6 = eButtonsDataVec{
            {eBuildingMode::none,
             Language::zeusText(28, 137),
             p6, 0, 0, &coll.fPhilosophy, nullptr, p6spr},
            {eBuildingMode::gymnasium,
             Language::zeusText(28, 79),
             g6, cost13, 1, &coll.fGymnasium, nullptr},
            {eBuildingMode::none,
             Language::zeusText(28, 27),
             d6, 0, 2, &coll.fDrama, nullptr, d6spr},
            {eBuildingMode::stadium,
             Language::zeusText(28, 80),
             s6, cost14, 3, &coll.fStadium, nullptr}};
        ww6 = createDataWidget(mCultureDataW, buttonsVec6,
                               Language::zeusText(88, 6));
    }

    const std::vector<eSPR> t7spr = {eSPR{eBuildingMode::templeZeus, Language::zeusText(28, 84),
                                          eBuilding::sInitialMarbleCost(eBuildingType::templeZeus)},
                                     eSPR{eBuildingMode::templePoseidon, Language::zeusText(28, 85),
                                          eBuilding::sInitialMarbleCost(eBuildingType::templePoseidon)},
                                     eSPR{eBuildingMode::templeHades, Language::zeusText(28, 95),
                                          eBuilding::sInitialMarbleCost(eBuildingType::templeHades)},
                                     eSPR{eBuildingMode::templeHera, Language::zeusText(28, 96),
                                          eBuilding::sInitialMarbleCost(eBuildingType::templeHera)},
                                     eSPR{eBuildingMode::templeDemeter, Language::zeusText(28, 86),
                                          eBuilding::sInitialMarbleCost(eBuildingType::templeDemeter)},
                                     eSPR{eBuildingMode::templeAthena, Language::zeusText(28, 92),
                                          eBuilding::sInitialMarbleCost(eBuildingType::templeAthena)},
                                     eSPR{eBuildingMode::templeArtemis, Language::zeusText(28, 88),
                                          eBuilding::sInitialMarbleCost(eBuildingType::templeArtemis)},
                                     eSPR{eBuildingMode::templeApollo, Language::zeusText(28, 87),
                                          eBuilding::sInitialMarbleCost(eBuildingType::templeApollo)},
                                     eSPR{eBuildingMode::templeAtlas, Language::zeusText(28, 97),
                                          eBuilding::sInitialMarbleCost(eBuildingType::templeAtlas)},
                                     eSPR{eBuildingMode::templeAres, Language::zeusText(28, 89),
                                          eBuilding::sInitialMarbleCost(eBuildingType::templeAres)},
                                     eSPR{eBuildingMode::templeHephaestus, Language::zeusText(28, 93),
                                          eBuilding::sInitialMarbleCost(eBuildingType::templeHephaestus)},
                                     eSPR{eBuildingMode::templeAphrodite, Language::zeusText(28, 90),
                                          eBuilding::sInitialMarbleCost(eBuildingType::templeAphrodite)},
                                     eSPR{eBuildingMode::templeHermes, Language::zeusText(28, 91),
                                          eBuilding::sInitialMarbleCost(eBuildingType::templeHermes)},
                                     eSPR{eBuildingMode::templeDionysus, Language::zeusText(28, 94),
                                          eBuilding::sInitialMarbleCost(eBuildingType::templeDionysus)}};
    const auto t7 = [this, cmx, cmy, t7spr]()
    {
        openBuildWidget(cmx, cmy, t7spr);
    };

    const std::vector<eSPR> hs7spr = {eSPR{eBuildingMode::achillesHall, Language::zeusText(185, 8)},
                                      eSPR{eBuildingMode::atalantaHall, Language::zeusText(185, 14)},
                                      eSPR{eBuildingMode::bellerophonHall, Language::zeusText(185, 15)},
                                      eSPR{eBuildingMode::herculesHall, Language::zeusText(185, 9)},
                                      eSPR{eBuildingMode::jasonHall, Language::zeusText(185, 10)},
                                      eSPR{eBuildingMode::odysseusHall, Language::zeusText(185, 11)},
                                      eSPR{eBuildingMode::perseusHall, Language::zeusText(185, 12)},
                                      eSPR{eBuildingMode::theseusHall, Language::zeusText(185, 13)}};
    const auto hs7 = [this, cmx, cmy, hs7spr]()
    {
        openBuildWidget(cmx, cmy, hs7spr);
    };

    mMythDataW = new eMythologyDataWidget(*b, window());
    const auto buttonsVec7 = eButtonsDataVec{
        {eBuildingMode::none,
         Language::zeusText(28, 83),
         t7, 0, 0, &coll.fTemples,
         &coll.fPoseidonTemples,
         t7spr},
        {eBuildingMode::none,
         Language::zeusText(28, 125),
         hs7, 0, 1, &coll.fHeroShrines,
         &coll.fPoseidonHeroShrines,
         hs7spr}};
    const auto ww8 = createDataWidget(mMythDataW, buttonsVec7,
                                      Language::zeusText(88, 7));

    const std::vector<eSPR> f8spr = {eSPR{eBuildingMode::wall, Language::zeusText(28, 130)},
                                     eSPR{eBuildingMode::tower, Language::zeusText(28, 132)},
                                     eSPR{eBuildingMode::gatehouse, Language::zeusText(28, 131)}};
    const auto f8 = [this, cmx, cmy, f8spr]()
    {
        openBuildWidget(cmx, cmy, f8spr);
    };
    const std::vector<eSPR> mp8spr = {eSPR{eBuildingMode::armory, Language::zeusText(28, 135)},
                                      eSPR{eBuildingMode::horseRanch, Language::zeusText(28, 133)},
                                      eSPR{eBuildingMode::chariotFactory, Language::zeusText(28, 212)},
                                      eSPR{eBuildingMode::triremeWharf, Language::zeusText(28, 136)}};
    const auto mp8 = [this, cmx, cmy, mp8spr]()
    {
        openBuildWidget(cmx, cmy, mp8spr);
    };

    mMiltDataW = new eMilitaryDataWidget(*b, window());
    const auto buttonsVec8 = eButtonsDataVec{
        {eBuildingMode::none,
         Language::zeusText(28, 139),
         f8, 0, 0, &coll.fFortifications, nullptr, f8spr},
        {eBuildingMode::none,
         Language::zeusText(28, 140),
         mp8, 0, 1, &coll.fMilitaryProduction, nullptr, mp8spr}};
    const auto ww9 = createDataWidget(mMiltDataW, buttonsVec8,
                                      Language::zeusText(88, 8));

    const std::vector<eSPR> bb9spr = {eSPR{eBuildingMode::park, Language::zeusText(28, 128)},
                                      eSPR{eBuildingMode::waterPark, Language::zeusText(28, 25)},
                                      eSPR{eBuildingMode::doricColumn, Language::zeusText(28, 129)},
                                      eSPR{eBuildingMode::ionicColumn, Language::zeusText(28, 145)},
                                      eSPR{eBuildingMode::corinthianColumn, Language::zeusText(28, 146)},
                                      eSPR{eBuildingMode::avenue, Language::zeusText(28, 118)}};
    const auto bb9 = [this, cmx, cmy, bb9spr]()
    {
        openBuildWidget(cmx, cmy, bb9spr);
    };
    const std::vector<eSPR> r9spr = {eSPR{eBuildingMode::bench, Language::zeusText(28, 127)},
                                     eSPR{eBuildingMode::birdBath, Language::zeusText(28, 152)},
                                     eSPR{eBuildingMode::shortObelisk, Language::zeusText(28, 24)},
                                     eSPR{eBuildingMode::tallObelisk, Language::zeusText(28, 19)},
                                     eSPR{eBuildingMode::flowerGarden, Language::zeusText(28, 15)},
                                     eSPR{eBuildingMode::gazebo, Language::zeusText(28, 16)},
                                     eSPR{eBuildingMode::shellGarden, Language::zeusText(28, 150)},
                                     eSPR{eBuildingMode::sundial, Language::zeusText(28, 20)},
                                     eSPR{eBuildingMode::hedgeMaze, Language::zeusText(28, 17)},
                                     eSPR{eBuildingMode::dolphinSculpture, Language::zeusText(28, 148)},
                                     eSPR{eBuildingMode::orrery, Language::zeusText(28, 149)},
                                     eSPR{eBuildingMode::spring, Language::zeusText(28, 22)},
                                     eSPR{eBuildingMode::topiary, Language::zeusText(28, 21)},
                                     eSPR{eBuildingMode::fishPond, Language::zeusText(28, 18)},
                                     eSPR{eBuildingMode::baths, Language::zeusText(28, 151)},
                                     eSPR{eBuildingMode::stoneCircle, Language::zeusText(28, 23)}};
    const auto r9 = [this, cmx, cmy, r9spr]()
    {
        openBuildWidget(cmx, cmy, r9spr);
    };
    const std::vector<eSPR> m9spr = {eSPR{eBuildingMode::populationMonument, Language::zeusText(198, 1)},
                                     eSPR{eBuildingMode::victoryMonument, Language::zeusText(198, 2)},
                                     eSPR{eBuildingMode::colonyMonument, Language::zeusText(198, 3)},
                                     eSPR{eBuildingMode::athleteMonument, Language::zeusText(198, 4)},
                                     eSPR{eBuildingMode::conquestMonument, Language::zeusText(198, 5)},
                                     eSPR{eBuildingMode::happinessMonument, Language::zeusText(198, 6)},
                                     eSPR{eBuildingMode::heroicFigureMonument, Language::zeusText(198, 7)},
                                     eSPR{eBuildingMode::diplomacyMonument, Language::zeusText(198, 8)},
                                     eSPR{eBuildingMode::scholarMonument, Language::zeusText(198, 9)},

                                     eSPR{eBuildingMode::aphroditeMonument, Language::zeusText(198, 16)},
                                     eSPR{eBuildingMode::apolloMonument, Language::zeusText(198, 13)},
                                     eSPR{eBuildingMode::aresMonument, Language::zeusText(198, 15)},
                                     eSPR{eBuildingMode::artemisMonument, Language::zeusText(198, 14)},
                                     eSPR{eBuildingMode::athenaMonument, Language::zeusText(198, 18)},
                                     eSPR{eBuildingMode::atlasMonument, Language::zeusText(198, 35)},
                                     eSPR{eBuildingMode::demeterMonument, Language::zeusText(198, 12)},
                                     eSPR{eBuildingMode::dionysusMonument, Language::zeusText(198, 20)},
                                     eSPR{eBuildingMode::hadesMonument, Language::zeusText(198, 21)},
                                     eSPR{eBuildingMode::hephaestusMonument, Language::zeusText(198, 19)},
                                     eSPR{eBuildingMode::heraMonument, Language::zeusText(198, 34)},
                                     eSPR{eBuildingMode::hermesMonument, Language::zeusText(198, 17)},
                                     eSPR{eBuildingMode::poseidonMonument, Language::zeusText(198, 11)},
                                     eSPR{eBuildingMode::zeusMonument, Language::zeusText(198, 10)}};
    const auto m9 = [this, cmx, cmy, m9spr]()
    {
        openBuildWidget(cmx, cmy, m9spr);
    };

    const std::vector<eSPR> p9spr = {eSPR{eBuildingMode::modestPyramid, Language::zeusText(28, 100)},
                                     eSPR{eBuildingMode::pyramid, Language::zeusText(28, 101)},
                                     eSPR{eBuildingMode::greatPyramid, Language::zeusText(28, 102)},
                                     eSPR{eBuildingMode::majesticPyramid, Language::zeusText(28, 103)},

                                     eSPR{eBuildingMode::smallMonumentToTheSky, Language::zeusText(28, 104)},
                                     eSPR{eBuildingMode::monumentToTheSky, Language::zeusText(28, 105)},
                                     eSPR{eBuildingMode::grandMonumentToTheSky, Language::zeusText(28, 106)},

                                     eSPR{eBuildingMode::minorShrineAphrodite, God::sGodName(GodType::aphrodite) + " " + Language::zeusText(28, 107)},
                                     eSPR{eBuildingMode::minorShrineApollo, God::sGodName(GodType::apollo) + " " + Language::zeusText(28, 107)},
                                     eSPR{eBuildingMode::minorShrineAres, God::sGodName(GodType::ares) + " " + Language::zeusText(28, 107)},
                                     eSPR{eBuildingMode::minorShrineArtemis, God::sGodName(GodType::artemis) + " " + Language::zeusText(28, 107)},
                                     eSPR{eBuildingMode::minorShrineAthena, God::sGodName(GodType::athena) + " " + Language::zeusText(28, 107)},
                                     eSPR{eBuildingMode::minorShrineAtlas, God::sGodName(GodType::atlas) + " " + Language::zeusText(28, 107)},
                                     eSPR{eBuildingMode::minorShrineDemeter, God::sGodName(GodType::demeter) + " " + Language::zeusText(28, 107)},
                                     eSPR{eBuildingMode::minorShrineDionysus, God::sGodName(GodType::dionysus) + " " + Language::zeusText(28, 107)},
                                     eSPR{eBuildingMode::minorShrineHades, God::sGodName(GodType::hades) + " " + Language::zeusText(28, 107)},
                                     eSPR{eBuildingMode::minorShrineHephaestus, God::sGodName(GodType::hephaestus) + " " + Language::zeusText(28, 107)},
                                     eSPR{eBuildingMode::minorShrineHera, God::sGodName(GodType::hera) + " " + Language::zeusText(28, 107)},
                                     eSPR{eBuildingMode::minorShrineHermes, God::sGodName(GodType::hermes) + " " + Language::zeusText(28, 107)},
                                     eSPR{eBuildingMode::minorShrinePoseidon, God::sGodName(GodType::poseidon) + " " + Language::zeusText(28, 107)},
                                     eSPR{eBuildingMode::minorShrineZeus, God::sGodName(GodType::zeus) + " " + Language::zeusText(28, 107)},

                                     eSPR{eBuildingMode::shrineAphrodite, God::sGodName(GodType::aphrodite) + " " + Language::zeusText(28, 108)},
                                     eSPR{eBuildingMode::shrineApollo, God::sGodName(GodType::apollo) + " " + Language::zeusText(28, 108)},
                                     eSPR{eBuildingMode::shrineAres, God::sGodName(GodType::ares) + " " + Language::zeusText(28, 108)},
                                     eSPR{eBuildingMode::shrineArtemis, God::sGodName(GodType::artemis) + " " + Language::zeusText(28, 108)},
                                     eSPR{eBuildingMode::shrineAthena, God::sGodName(GodType::athena) + " " + Language::zeusText(28, 108)},
                                     eSPR{eBuildingMode::shrineAtlas, God::sGodName(GodType::atlas) + " " + Language::zeusText(28, 108)},
                                     eSPR{eBuildingMode::shrineDemeter, God::sGodName(GodType::demeter) + " " + Language::zeusText(28, 108)},
                                     eSPR{eBuildingMode::shrineDionysus, God::sGodName(GodType::dionysus) + " " + Language::zeusText(28, 108)},
                                     eSPR{eBuildingMode::shrineHades, God::sGodName(GodType::hades) + " " + Language::zeusText(28, 108)},
                                     eSPR{eBuildingMode::shrineHephaestus, God::sGodName(GodType::hephaestus) + " " + Language::zeusText(28, 108)},
                                     eSPR{eBuildingMode::shrineHera, God::sGodName(GodType::hera) + " " + Language::zeusText(28, 108)},
                                     eSPR{eBuildingMode::shrineHermes, God::sGodName(GodType::hermes) + " " + Language::zeusText(28, 108)},
                                     eSPR{eBuildingMode::shrinePoseidon, God::sGodName(GodType::poseidon) + " " + Language::zeusText(28, 108)},
                                     eSPR{eBuildingMode::shrineZeus, God::sGodName(GodType::zeus) + " " + Language::zeusText(28, 108)},

                                     eSPR{eBuildingMode::majorShrineAphrodite, God::sGodName(GodType::aphrodite) + " " + Language::zeusText(28, 109)},
                                     eSPR{eBuildingMode::majorShrineApollo, God::sGodName(GodType::apollo) + " " + Language::zeusText(28, 109)},
                                     eSPR{eBuildingMode::majorShrineAres, God::sGodName(GodType::ares) + " " + Language::zeusText(28, 109)},
                                     eSPR{eBuildingMode::majorShrineArtemis, God::sGodName(GodType::artemis) + " " + Language::zeusText(28, 109)},
                                     eSPR{eBuildingMode::majorShrineAthena, God::sGodName(GodType::athena) + " " + Language::zeusText(28, 109)},
                                     eSPR{eBuildingMode::majorShrineAtlas, God::sGodName(GodType::atlas) + " " + Language::zeusText(28, 109)},
                                     eSPR{eBuildingMode::majorShrineDemeter, God::sGodName(GodType::demeter) + " " + Language::zeusText(28, 109)},
                                     eSPR{eBuildingMode::majorShrineDionysus, God::sGodName(GodType::dionysus) + " " + Language::zeusText(28, 109)},
                                     eSPR{eBuildingMode::majorShrineHades, God::sGodName(GodType::hades) + " " + Language::zeusText(28, 109)},
                                     eSPR{eBuildingMode::majorShrineHephaestus, God::sGodName(GodType::hephaestus) + " " + Language::zeusText(28, 109)},
                                     eSPR{eBuildingMode::majorShrineHera, God::sGodName(GodType::hera) + " " + Language::zeusText(28, 109)},
                                     eSPR{eBuildingMode::majorShrineHermes, God::sGodName(GodType::hermes) + " " + Language::zeusText(28, 109)},
                                     eSPR{eBuildingMode::majorShrinePoseidon, God::sGodName(GodType::poseidon) + " " + Language::zeusText(28, 109)},
                                     eSPR{eBuildingMode::majorShrineZeus, God::sGodName(GodType::zeus) + " " + Language::zeusText(28, 109)},

                                     eSPR{eBuildingMode::pyramidToThePantheon, Language::zeusText(28, 110)},
                                     eSPR{eBuildingMode::altarOfOlympus, Language::zeusText(28, 111)},
                                     eSPR{eBuildingMode::templeOfOlympus, Language::zeusText(28, 112)},
                                     eSPR{eBuildingMode::observatoryKosmika, Language::zeusText(28, 113)},
                                     eSPR{eBuildingMode::museumAtlantika, Language::zeusText(28, 114)}};
    const auto p9 = [this, cmx, cmy, p9spr]()
    {
        openBuildWidget(cmx, cmy, p9spr);
    };

    mApplDataW = new eAppealDataWidget(*b, window());
    const auto buttonsVec = eButtonsDataVec{
        {eBuildingMode::none,
         Language::zeusText(28, 142),
         bb9, 0, 0, &coll.fBeautification, nullptr, bb9spr},
        {eBuildingMode::none,
         Language::zeusText(28, 141),
         r9, 0, 1, &coll.fRecreation, nullptr, r9spr},
        {eBuildingMode::none,
         Language::zeusText(28, 157),
         m9, 0, 2, &coll.fMonuments, nullptr, m9spr},
        {eBuildingMode::none,
         Language::zeusText(28, 157),
         p9, 0, 3, &coll.fPiramids,
         nullptr,
         p9spr}};
    const auto ww10 = createDataWidget(mApplDataW, buttonsVec,
                                       Language::zeusText(88, 9));

    mOverDataW = new OverviewDataWidget(*b, window());
    mMiniMap = new eMiniMap(window());
    mMiniMap->setTDim(1.25 * mult);
    mOverDataW->setMap(mMiniMap);
    mMiniMap->resize(dataWidWidth, dataWidWidth * 0.85);

    const auto ww11 = createDataWidgetBase(mOverDataW, mMiniMap,
                                           Language::zeusText(88, 10));
    mWidgets.push_back({ww0, mPopDataW});
    mWidgets.push_back({ww1, mHusbDataW});
    mWidgets.push_back({ww2, mEmplDataW});
    mWidgets.push_back({www3, mStrgDataW});
    mWidgets.push_back({ww4, mHySaDataW});
    mWidgets.push_back({ww5, mAdminDataW});
    mWidgets.push_back({ww6, static_cast<eDataWidget *>(mCultureDataW)});
    mWidgets.push_back({ww7, static_cast<eDataWidget *>(mScienceDataW)});
    mWidgets.push_back({ww8, mMythDataW});
    mWidgets.push_back({ww9, mMiltDataW});
    mWidgets.push_back({ww10, mApplDataW});
    mWidgets.push_back({ww11, mOverDataW});

    for (const auto &ww : mWidgets)
    {
        const auto w = ww.fW;
        addWidget(w);
        w->move(wx, 12 * mult);
        w->hide();
    }

    const auto b0 = addButton(coll.fPopulation, mWidgets[0]);
    mPopulationButton = b0;
    const auto b1 = addButton(coll.fHusbandry, mWidgets[1]);
    mHusbandryButton = b1;
    const auto b2 = addButton(coll.fIndustry, mWidgets[2]);
    mIndustryButton = b2;
    const auto b3 = addButton(coll.fDistribution, mWidgets[3]);
    mDistributionButton = b3;
    const auto b4 = addButton(coll.fHygieneSafety, mWidgets[4]);
    mHygieneSafetyButton = b4;
    const auto b5 = addButton(coll.fAdministration, mWidgets[5]);
    mAdministrationButton = b5;
    const auto b6a = addButton(coll.fCulture, mWidgets[6]);
    mCultureButton = b6a;
    const auto b7 = addButton(coll.fMythology, mWidgets[8]);
    mMythologyButton = b7;
    const auto b8 = addButton(coll.fMilitary, mWidgets[9]);
    mMilitaryButton = b8;
    const auto b9 = addButton(coll.fAesthetics, mWidgets[10]);
    mAesthethicsButton = b9;
    const auto b10 = addButton(coll.fOverview, mWidgets[11]);
    mOverviewButton = b10;

    const auto setupButtonHover =
        [this](eCheckableButton *b0, const std::string &txt)
    {
        b0->setMouseEnterAction([this, txt]()
                                { mNameLabel->setText(txt); });
        b0->setMouseLeaveAction([this]()
                                { mNameLabel->setText(""); });
    };

    setupButtonHover(b0, Language::zeusText(88, 0));
    setupButtonHover(b1, Language::zeusText(88, 1));
    setupButtonHover(b2, Language::zeusText(88, 2));
    setupButtonHover(b3, Language::zeusText(88, 3));
    setupButtonHover(b4, Language::zeusText(88, 4));
    setupButtonHover(b5, Language::zeusText(88, 5));
    setupButtonHover(b6a, Language::zeusText(88, 6));
    setupButtonHover(b7, Language::zeusText(88, 7));
    setupButtonHover(b8, Language::zeusText(88, 8));
    setupButtonHover(b9, Language::zeusText(88, 9));
    setupButtonHover(b10, Language::zeusText(88, 10));

    b10->setChecked(true);
    ww11->setVisible(true);

    layoutButtons();

    const auto b6b = addButton(coll.fScience, mWidgets[7]);
    setupButtonHover(b6b, Language::zeusText(88, 24));
    b6b->hide();
    b6b->move(b6a->x(), b6a->y());
    mScienceButton = b6b;

    connectButtons();

    {
        const auto btmButtons = new eWidget(window());
        btmButtons->setPadding(0);

        const auto b = eButton::sCreate(coll.fBuildRoad, window(), btmButtons);
        b->setPressAction([this]()
                          { setMode(eBuildingMode::road); });
        const auto rb = eButton::sCreate(coll.fRoadBlock, window(), btmButtons);
        rb->setPressAction([this]()
                           { setMode(eBuildingMode::roadblock); });
        const auto e = eButton::sCreate(coll.fClear, window(), btmButtons);
        e->setPressAction([this]()
                          { setMode(eBuildingMode::erase); });
        mUndoButton = eButton::sCreate(coll.fUndo, window(), btmButtons);
        mUndoButton->setPressAction([this]()
                                    {
            mBoard->undoLastAction();
            update(); });

        const int x = mult * 24;
        const int y = std::round(mult * 217.5);
        btmButtons->resize(4 * b->width(), b->height());
        btmButtons->move(x, y);
        btmButtons->layoutHorizontally();
        addWidget(btmButtons);
    }

    {
        const auto butts = new eWidget(window());
        const auto info = eCheckableButton::sCreate(coll.fShowInfo, window(), butts);
        const auto map = eCheckableButton::sCreate(coll.fShowMap, window(), butts);
        info->setChecked(true);
        info->setCheckAction([info, map](const bool c)
                             {
            if(!c) return info->setChecked(true);
            map->setChecked(false); });
        map->setCheckAction([info, map](const bool c)
                            {
            if(!c) return map->setChecked(true);
            info->setChecked(false); });
        butts->resize(info->width() + map->width(), info->height());
        butts->layoutHorizontally();
        butts->setX(mult * 26);
        addWidget(butts);
    }

    {
        mMessagesButton = eButton::sCreate(coll.fMessages, window(), this);
        mMessagesButton->setPressAction([this]()
                                        {
            if(mMsgListW) {
                if(mMsgListW->visible()) {
                    mMsgListW->hide();
                    if(!mMsgListWasPaused) mGW->pauseGame();
                } else {
                    mMsgListWasPaused = mGW->gamePaused();
                    if(!mMsgListWasPaused) mGW->pauseGame();
                    mMsgListW->align(Alignment::center);
                    mMsgListW->show();
                }
            } });
        mMessagesButton->move(mult * 73, mult * 239);

        mMsgBadge = new eLabel(window());
        mMsgBadge->setFontSizeS();
        mMsgBadge->setPaddingXS();
        mMsgBadge->setVisible(false);
        addWidget(mMsgBadge);
    }
    {
        class eBlueWidget : public eWidget
        {
        public:
            eBlueWidget(MainWindow *const w) : eWidget(w) {}
            void paintEvent(ePainter &p) override
            {
                p.fillRect({0, 0, width(), height()}, {11, 75, 110, 255});
            }
        };
        const auto butts = new eBlueWidget(window());
        butts->setPadding(0);

        const auto goals = new eBasicButton(&InterfaceTextures::fGoals, window());
        goals->setTooltip(Language::zeusText(68, 9));
        butts->addWidget(goals);
        goals->setPressAction(goalsView);

        mRotateButton = new eRotateButton(window());
        butts->addWidget(mRotateButton);

        mWorldButton = eButton::sCreate(coll.fWorld, window(), butts);
        mWorldButton->setTooltip(Language::zeusText(68, 17));

        const int w = goals->width() + mRotateButton->width() + mWorldButton->width();
        butts->resize(w, mWorldButton->height());
        butts->layoutHorizontallyWithoutSpaces();
        butts->setX(mult * 5.5);
        butts->setY(std::round(mult * 282.5));
        addWidget(butts);
    }

    {
        mEventW = new eEventWidget(window());
        mEventW->setNoPadding();
        mEventW->setX(mult * 5);
        mEventW->setY(mult * 240);
        mEventW->setWidth(dataWidWidth);
        addWidget(mEventW);
    }

    mMiniMap->setBoard(b);

    updateButtonsVisibility();
    update();
}

void GameMenu::setGameWidget(GameWidget *const gw)
{
    mGW = gw;
    mPopDataW->setGameWidget(gw);
    mHusbDataW->setGameWidget(gw);
    mEmplDataW->setGameWidget(gw);
    mStrgDataW->setGameWidget(gw);
    mApplDataW->setGameWidget(gw);
    mHySaDataW->setGameWidget(gw);
    mMiltDataW->setGameWidget(gw);
    mMythDataW->setGameWidget(gw);
    mAdminDataW->setGameWidget(gw);
    if (mCultureDataW)
        mCultureDataW->setGameWidget(gw);
    if (mScienceDataW)
        mScienceDataW->setGameWidget(gw);
    mOverDataW->setGameWidget(gw);

    {
        const auto mlw = new eMessageListWidget(window());
        const int mlwW = gw->width() / 3;
        const int mlwH = gw->height() / 2;
        mlw->resize(mlwW, mlwH);
        mlw->initialize([gw](eEventData ed, const Message &msg)
                        { gw->showMessage(ed, msg, false, true, false); }, [this]()
                        {
            if(!mMsgListWasPaused) mGW->pauseGame(); });
        mlw->setVisible(false);
        mlw->setUnreadChangedAction([this](const int n)
                                    {
            if(!mMsgBadge) return;
            if(n <= 0) {
                mMsgBadge->setVisible(false);
                return;
            }
            mMsgBadge->setText(std::to_string(n));
            mMsgBadge->fitContent();
            const int bx = mMessagesButton->x() + (mMessagesButton->width() - mMsgBadge->width()) / 2;
            const int by = mMessagesButton->y() + (mMessagesButton->height() - mMsgBadge->height()) / 2;
            mMsgBadge->move(bx, by - 2);
            mMsgBadge->setVisible(true); });
        gw->addWidget(mlw);
        gw->setMessageListWidget(mlw);
        mMsgListW = mlw;
    }

    mWorldButton->setPressAction([this]()
                                 { window()->showWorld(); });

    mRotateButton->setDirectionSetter([gw](const eWorldDirection dir)
                                      { gw->setWorldDirection(dir); });
    updateButtonsVisibility();
}

eMiniMap *GameMenu::miniMap() const
{
    return mMiniMap;
}

void GameMenu::pushEvent(const eEvent e, const eEventData &ed)
{
    mEventW->pushEvent(e, ed);
}

void GameMenu::setViewTileHandler(const eViewTileHandler &h)
{
    mEventW->setViewTileHandler(h);
}

void GameMenu::closeBuildWidget()
{
    if (!mBuildWidget)
        return;
    mBuildWidget->deleteLater();
    mBuildWidget = nullptr;
}

void GameMenu::setBuildWidget(eBuildWidget *const bw)
{
    closeBuildWidget();
    mBuildWidget = bw;
}

void GameMenu::updateButtonsVisibility()
{
    const auto cid = mGW ? mGW->viewedCity() : eCityId::neutralFriendly;
    for (const auto s : mSubButtons)
    {
        s->updateVisible(cid, mShowAllPossibleBuildings);
    }
    const auto c = mBoard->boardCityWithId(cid);
    const bool science = c ? c->atlantean() : false;

    const auto pid = mBoard->cityIdToPlayerId(cid);
    const auto ppid = mBoard->personPlayer();
    const bool e = (c && pid == ppid) || mShowAllPossibleBuildings;

    mPopulationButton->setEnabled(e);
    mHusbandryButton->setEnabled(e);
    mIndustryButton->setEnabled(e);
    mDistributionButton->setEnabled(e);
    mHygieneSafetyButton->setEnabled(e);
    mAdministrationButton->setEnabled(e);
    mScienceButton->setEnabled(e);
    mCultureButton->setEnabled(e);
    mMythologyButton->setEnabled(e);
    mMilitaryButton->setEnabled(e);
    mAesthethicsButton->setEnabled(e);
    mOverviewButton->setEnabled(e);

    mScienceButton->setVisible(science);
    mCultureButton->setVisible(!science);
    if (science && mCultureButton->checked())
    {
        mScienceButton->trigger();
    }
    else if (!science && mScienceButton->checked())
    {
        mCultureButton->trigger();
    }
}

void GameMenu::viewedCityChanged()
{
    mPopDataW->update();
    mHusbDataW->update();
    mEmplDataW->update();
    mStrgDataW->update();
    mApplDataW->update();
    mHySaDataW->update();
    mMiltDataW->update();
    mMythDataW->update();
    mAdminDataW->update();
    if (mCultureDataW)
        mCultureDataW->update();
    if (mScienceDataW)
        mScienceDataW->update();
    mOverDataW->update();
    updateButtonsVisibility();
    setMode(eBuildingMode::none);
}

void GameMenu::setMode(const eBuildingMode mode)
{
    closeBuildWidget();
    mMode = mode;
    if (mModeChangeAct)
        mModeChangeAct();
}

bool GameMenu::mousePressEvent(const eMouseEvent &e)
{
    closeBuildWidget();
    return GameMenuBase::mouseEnterEvent(e);
}
