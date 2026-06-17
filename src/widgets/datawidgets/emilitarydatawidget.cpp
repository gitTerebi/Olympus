#include "emilitarydatawidget.h"

#include "eviewmodebutton.h"

#include "engine/game-board.h"
#include "language.h"
#include "vector-helpers.h"
#include "widgets/emicrobutton.h"
#include "widgets/ebasicbutton.h"
#include "widgets/emilitarymoreinfowidget.h"
#include "widgets/escrollwidget.h"
#include "characters/soldier-banner.h"

void eForcesWidget::initialize(const std::string& title) {
    setNoPadding();
    const auto titleLabel = new eLabel(window());
    titleLabel->setFontSizeXS();
    titleLabel->setNoPadding();
    titleLabel->setText(title);
    titleLabel->fitContent();
    addWidget(titleLabel);
    titleLabel->align(Alignment::hcenter);

    mWidget = new eWidget(window());
    mWidget->setNoPadding();
    addWidget(mWidget);

    stackVertically();
    fitHeight();
}

void eForcesWidget::setBanners(const SoldierBanners& ss,
                               const SoldierBannerAction& act) {
    const bool changed = !VectorHelpers::same(mBanners, ss);
    if(!changed) return;
    mBanners = ss;
    const int iRes = GameTextures::interfaceTextureId();
    const auto& intrfc = GameTextures::interface();
    const auto& coll = intrfc[iRes];
    const auto& tops = coll.fInterfaceBannerTops;
    const auto& pTops = coll.fPoseidonInterfaceBannerTops;

    mWidget->removeChildren();
    eWidget* line = nullptr;
    int lineI = 0;
    const auto finishLine = [&]() {
        if(!line) return;
        line->stackHorizontally();
        line->fitContent();
        mWidget->addWidget(line);
        line = nullptr;
        lineI = 0;
    };
    for(const auto& s : ss) {
        if(!line) {
            line = new eWidget(window());
            line->setNoPadding();
        }
        const auto button = new eButtonBase(window());
        button->setNoPadding();
        button->setTooltip(s->name());
        button->setPressAction([s, act]() {
            if(act && s) act(s);
        });

        {
            int topId = 0;
            const auto sType = s->type();
            const bool p = s->atlantean();
            if(!p || sType == eBannerType::amazon ||
               sType == eBannerType::aresWarrior) {
                switch(sType) {
                case eBannerType::horseman:
                    topId = 0;
                    break;
                case eBannerType::hoplite:
                    topId = 1;
                    break;
                case eBannerType::rockThrower:
                    topId = 2;
                    break;
                case eBannerType::amazon:
                    topId = 4;
                    break;
                case eBannerType::aresWarrior:
                    topId = 5;
                    break;
                default:
                    break;
                }

                const auto& top = tops.getTexture(topId);
                button->setTexture(top);
            } else {
                switch(sType) {
                case eBannerType::horseman:
                    topId = 0;
                    break;
                case eBannerType::rockThrower:
                    topId = 1;
                    break;
                case eBannerType::hoplite:
                    topId = 2;
                    break;
                default:
                    break;
                }

                const auto& top = pTops.getTexture(topId);
                button->setTexture(top);
            }
        }
        button->setTextureDrawScale(topSidebarTextureScale());
        button->fitContent();
        line->addWidget(button);
        lineI++;
        if(lineI >= 5) {
            finishLine();
        }
    }
    finishLine();
    mWidget->stackVertically();
    mWidget->fitContent();
    stackVertically();
    fitHeight();
}

void eMilitaryDataWidget::initialize() {
    {
        mSeeSecurity = new eViewModeButton(
                        Language::zeusText(14, 16),
                        eViewMode::security,
                        window());
        addViewButton(mSeeSecurity);
    }
    const int iRes = GameTextures::interfaceTextureId();
    const auto& intrfc = GameTextures::interface()[iRes];

    eDataWidget::initialize();

    const auto inner = innerWidget();
    const int iw = inner->width();
    const int ih = inner->height();

    mForcesScroll = new eScrollWidget(window());

    mForcesWidget = new eWidget(window());
    mForcesWidget->setNoPadding();
    mForcesWidget->setWidth(iw);

    mAbroad = new eForcesWidget(window());
    mAbroad->setWidth(iw);
    mAbroad->initialize(Language::zeusText(51, 1)); // forces abroad
    mForcesWidget->addWidget(mAbroad);
    mAbroad->hide();

    mInCity = new eForcesWidget(window());
    mInCity->setWidth(iw);
    mInCity->initialize(Language::zeusText(51, 0)); // forces in city
    mForcesWidget->addWidget(mInCity);
    mInCity->hide();

    mStandingDown = new eForcesWidget(window());
    mStandingDown->setWidth(iw);
    mStandingDown->initialize(Language::zeusText(51, 2)); // standing down
    mForcesWidget->addWidget(mStandingDown);
    mStandingDown->hide();

    mForcesWidget->stackVertically();
    mForcesWidget->fitHeight();
    mForcesScroll->setScrollArea(mForcesWidget);
    inner->addWidget(mForcesScroll);

    const auto buttonsW = new eWidget(window());
    buttonsW->setNoPadding();
    buttonsW->setWidth(iw);

    const double scale = topSidebarTextureScale();

    const int microW = std::round(84*scale);
    const auto microButtonsW = new eWidget(window());
    microButtonsW->setNoPadding();
    microButtonsW->setWidth(microW);
    mAtPalace = new eMicroButton(window());
    mAtPalace->setTextureDrawScale(scale);
    mAtPalace->setNoPadding();
    mAtPalace->setFontSizeXS();
    mAtPalace->setText(Language::zeusText(51, 82));
    mAtPalace->setTooltip(Language::zeusText(68, 37));
    mAtPalace->setWidth(microW);
    mAtPalace->fitHeight();
    microButtonsW->addWidget(mAtPalace);

    const int iconX = std::round(-10*scale);
    const auto soldiersIcon = new eLabel(window());
    soldiersIcon->setTextureDrawScale(scale);
    soldiersIcon->setTexture(intrfc.fSoldiersIcon);
    soldiersIcon->fitContent();
    mAtPalace->addWidget(soldiersIcon);
    soldiersIcon->setX(iconX - soldiersIcon->width()/2);
    soldiersIcon->align(Alignment::vcenter);

    mNoShips = new eMicroButton(window());
    mNoShips->setTextureDrawScale(scale);
    mNoShips->setNoPadding();
    mNoShips->setFontSizeXS();
    mNoShips->setText(Language::zeusText(51, 83));
    mNoShips->setTooltip(Language::zeusText(68, 38));
    mNoShips->setWidth(microW);
    mNoShips->fitHeight();
    microButtonsW->addWidget(mNoShips);

    const auto shipsIcon = new eLabel(window());
    shipsIcon->setTextureDrawScale(scale);
    shipsIcon->setTexture(intrfc.fShipsIcon);
    shipsIcon->fitContent();
    mNoShips->addWidget(shipsIcon);
    shipsIcon->setX(iconX - shipsIcon->width()/2);
    shipsIcon->align(Alignment::vcenter);

    mNoTowers = new eMicroButton(window());
    mNoTowers->setTextureDrawScale(scale);
    mNoTowers->setNoPadding();
    mNoTowers->setFontSizeXS();
    mNoTowers->setText(Language::zeusText(51, 84));
    mNoTowers->setTooltip(Language::zeusText(68, 39));
    mNoTowers->setWidth(microW);
    mNoTowers->fitHeight();
    microButtonsW->addWidget(mNoTowers);

    const auto towersIcon = new eLabel(window());
    towersIcon->setTextureDrawScale(scale);
    towersIcon->setTexture(intrfc.fTowersIcon);
    towersIcon->fitContent();
    mNoTowers->addWidget(towersIcon);
    towersIcon->setX(iconX - towersIcon->width()/2);
    towersIcon->align(Alignment::vcenter);

    const int microP = std::round(4*scale);
    microButtonsW->stackVertically(microP);
    microButtonsW->fitHeight();
    buttonsW->addWidget(microButtonsW);
    microButtonsW->setX(std::round(18*scale));

    const auto coll = &InterfaceTextures::fMilitaryControlManual;
    const auto controlButton = new eBasicButton(coll, window());
    controlButton->setTextureDrawScale(scale);
    controlButton->fitContent();
    buttonsW->addWidget(controlButton);
    controlButton->setX(std::round(106*scale));

    buttonsW->fitHeight();
    inner->addWidget(buttonsW);
    buttonsW->align(Alignment::bottom);

    const int bh = buttonsW->height();
    mForcesScroll->resize(iw, ih - bh);
    mForcesScroll->initializeButtons();

    showMoreInfoButton();
}

void eMilitaryDataWidget::paintEvent(ePainter& p) {
    const bool update = ((mTime++) % 20) == 0;
    if(update) {
        updateWidgets();
    }
    eWidget::paintEvent(p);
}

void eMilitaryDataWidget::openMoreInfoWiget() {
    const auto gw = gameWidget();
    const auto w = new eMilitaryMoreInfoWidget(window(), gw);
    w->initialize(mBoard, viewedCity());
    gw->openDialog(w);
}

void eMilitaryDataWidget::updateWidgets() {
    const auto cid = viewedCity();
    const auto ss = mBoard.banners(cid);
    using SoldierBanners = std::vector<stdptr<SoldierBanner>>;
    SoldierBanners abroad;
    SoldierBanners inCity;
    SoldierBanners standingDown;
    for(const auto& s : ss) {
        if(s->isAbroad()) {
            abroad.push_back(s);
        } else if(s->isHome()) {
            standingDown.push_back(s);
        } else {
            inCity.push_back(s);
        }
    }
    const auto act = [this](const eSB& s) {
        mBoard.clearBannerSelection();
        mBoard.selectBanner(s.get());
        const auto t = s->tile();
        const auto gw = gameWidget();
        gw->viewTile(t);
    };
    mAbroad->setBanners(abroad, act);
    if(abroad.empty()) {
        mAbroad->hide();
        mAbroad->setHeight(0);
    } else {
        mAbroad->show();
    }
    mInCity->setBanners(inCity, act);
    if(inCity.empty()) {
        mInCity->hide();
        mInCity->setHeight(0);
    } else {
        mInCity->show();
    }
    mStandingDown->setBanners(standingDown, act);
    if(standingDown.empty()) {
        mStandingDown->hide();
        mStandingDown->setHeight(0);
    } else {
        mStandingDown->show();
    }
    mForcesWidget->stackVertically();
    mForcesWidget->fitHeight();

    const auto towers = mBoard.buildings(cid, eBuildingType::tower);

    if(inCity.empty() && standingDown.empty()) {
        mAtPalace->setText(Language::zeusText(51, 82)); // no soldiers
        mAtPalace->setTooltip(Language::zeusText(68, 37)); // no soldiers to command
        mAtPalace->setPressAction(nullptr);
    } else if(!inCity.empty()) {
        mAtPalace->setText(Language::zeusText(51, 6)); // all called
        mAtPalace->setTooltip(Language::zeusText(68, 170)); // click to send all soldiers home
        mAtPalace->setPressAction([this, cid]() {
            mBoard.sendAllSoldiersHome(cid);
            updateWidgets();
        });
    } else {
        mAtPalace->setText(Language::zeusText(51, 8)); // at palace
        mAtPalace->setTooltip(Language::zeusText(68, 171)); // click to muster all
        mAtPalace->setPressAction([this, cid]() {
            mBoard.musterAllSoldiers(cid);
            updateWidgets();
        });
    }
    if(towers.empty()) {
        mNoTowers->setText(Language::zeusText(51, 84)); // no towers
        mNoTowers->setTooltip(Language::zeusText(68, 39)); // no towers to man
        mNoTowers->setPressAction(nullptr);
    } else if(mBoard.manTowers(cid)) {
        mNoTowers->setText(Language::zeusText(51, 11)); // manning
        mNoTowers->setTooltip(Language::zeusText(68, 174)); // click to send home
        mNoTowers->setPressAction([this, cid]() {
            mBoard.setManTowers(cid, false);
            updateWidgets();
        });
    } else {
        mNoTowers->setText(Language::zeusText(51, 12)); // not manning
        mNoTowers->setTooltip(Language::zeusText(68, 175)); // click to man
        mNoTowers->setPressAction([this, cid]() {
            mBoard.setManTowers(cid, true);
            updateWidgets();
        });
    }

    mForcesScroll->clampDY();
}
