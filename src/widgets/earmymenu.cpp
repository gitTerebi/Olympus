#include "earmymenu.h"

#include "language.h"
#include "textures/game-textures.h"

#include "eminimap.h"
#include "ebasicbutton.h"
#include "eframedwidget.h"

#include "game-widget.h"
#include "characters/soldier-banner.h"

#include <cmath>

std::vector<SoldierBanner*> eArmyMenu::selectedPlayerBanners() const {
    std::vector<SoldierBanner*> result;
    if (!mBoard) return result;
    const auto& selectedBanners = mBoard->selectedSoldiers();
    const auto ppid = mBoard->personPlayer();
    for (const auto* b : selectedBanners) {
        if (b && b->playerId() == ppid) {
            result.push_back(const_cast<SoldierBanner*>(b));
        }
    }
    return result;
}

void eArmyMenu::initialize(GameBoard &b)
{
    mBoard = &b;

    int iRes;
    double mult;
    topSidebarIResAndMult(iRes, mult);

    const auto &intrfc = GameTextures::interface();
    const auto &coll = intrfc[iRes];
    const auto tex = coll.fGameArmyBackground;
    setTextureDrawScale(topSidebarTextureScale());
    setTexture(tex);
    setPadding(0);
    fitContent();

    const auto wid = new eWidget(window());
    wid->setNoPadding();
    wid->move(std::round(mult * 11.5), std::round(mult * 13));
    wid->resize(std::round(mult * 71), std::round(mult * 216));
    addWidget(wid);

    mMiniMap = new eMiniMap(window());
    wid->addWidget(mMiniMap);
    mMiniMap->resize(wid->width(), std::round(mult * 56));
    mMiniMap->setBoard(&b);

    const auto status = new eLabel(window());
    status->setNoPadding();
    status->setTextureDrawScale(topSidebarTextureScale());
    status->setTexture(coll.fArmyStatus);
    status->fitContent();
    wid->addWidget(status);
    status->resize(wid->width(), std::round(mult * 23));
    status->setX(std::round(mult * 0.75));
    status->setY(std::round(mult * 60.75));

    const auto rowY = [&](const int sourceY) {
        return std::round(topSidebarTextureScale() * sourceY) -
               std::round(mult * 13);
    };
    const int y1 = rowY(203);
    const int y2 = rowY(255);
    const int y3 = rowY(307);
    const auto applyArmyButtonScale = [this](eBasicButton* const button) {
        button->setTextureDrawScale(topSidebarTextureScale());
        const int scaleId = topSidebarScaleId();
        if(scaleId >= 3) {
            button->setTextureDrawBleed(4);
        } else if(scaleId >= 2) {
            button->setTextureDrawBleed(3);
        } else {
            button->setTextureDrawBleed(1);
        }
        button->fitContent();
    };

    const auto t1 = &InterfaceTextures::fGoToCompany;
    const auto cou = new eBasicButton(t1, window());
    applyArmyButtonScale(cou);
    wid->addWidget(cou);
    cou->setY(y1);
    cou->setTooltip(Language::zeusText(51, 70)); // Go To Company
    cou->setPressAction([this]()
                       {
    if (!mBoard || !mGW) return;
    const auto selectedPlayerBanners = this->selectedPlayerBanners();
    if (selectedPlayerBanners.empty()) return;
    
    // Use the first selected banner
    const auto companyBanner = selectedPlayerBanners[0];
    const auto t = companyBanner->tile();
    if (t) {
        mGW->viewTile(t);
    } });

    const auto t2 = &InterfaceTextures::fDefensiveTactics;
    const auto dt = new eBasicButton(t2, window());
    applyArmyButtonScale(dt);
    wid->addWidget(dt);
    dt->setY(y1);
    eBasicButton* ot = nullptr;
    eBasicButton* st = nullptr;
    const auto rightColumnX = [&]() {
        return std::round(mult * 49) - std::round(mult * 11.5);
    };
    const int xx = rightColumnX();
    dt->setX(xx);
    dt->setTooltip(Language::zeusText(51, 73)); // Defensive Tactics

    const auto t3 = &InterfaceTextures::fRotateCompany;
    const auto rc = new eBasicButton(t3, window());
    applyArmyButtonScale(rc);
    wid->addWidget(rc);
    rc->setY(y2);
    rc->setTooltip(Language::zeusText(51, 76)); // Rotate Company
    rc->setPressAction([this]()
                       {
        if (!mBoard || !mGW) return;
        SoldierBanner::sRotatePlayerBanners(
            mBoard->selectedSoldiers(), mBoard->personPlayer()); });

    const auto t4 = &InterfaceTextures::fOffensiveTactics;
    ot = new eBasicButton(t4, window());
    applyArmyButtonScale(ot);
    wid->addWidget(ot);
    ot->setY(y2);
    ot->setX(xx);
    ot->setTooltip(Language::zeusText(51, 74)); // Offensive Tactics

    const auto t5 = &InterfaceTextures::fGoToBanner;
    mGoToBanner = new eBasicButton(t5, window());
    applyArmyButtonScale(mGoToBanner);
    wid->addWidget(mGoToBanner);
    mGoToBanner->setY(y3);
    mGoToBanner->setTooltip(Language::zeusText(51, 25)); // Go To Banner (Muster)
    mGoToBanner->setPressAction([this]()
                                {
        mBoard->bannersBackFromHome();
        setSoldiersHome(false); });

    const auto t5_2 = &InterfaceTextures::fGoHome;
    mGoHome = new eBasicButton(t5_2, window());
    applyArmyButtonScale(mGoHome);
    wid->addWidget(mGoHome);
    mGoHome->setY(y3);
    mGoHome->setTooltip(Language::zeusText(51, 26)); // Go Home
    mGoHome->hide();
    mGoHome->setPressAction([this]()
                            {
        mBoard->bannersGoHome();
        setSoldiersHome(true); });

    const auto t6 = &InterfaceTextures::fSpecialTactics;
    st = new eBasicButton(t6, window());
    applyArmyButtonScale(st);
    wid->addWidget(st);
    st->setY(y3);
    st->setX(xx);
    st->setTooltip(Language::zeusText(51, 75)); // Special Tactics

    const auto ww = new eFramedWidget(window());
    wid->addWidget(ww);
    ww->setType(eFrameType::inner);
    ww->setY(y3 + cou->height() + std::round(mult * 2));
    ww->resize(wid->width(), std::round(mult * 50));
}

void eArmyMenu::setSoldiersHome(const bool h)
{
    mGoToBanner->setVisible(h);
    mGoHome->setVisible(!h);
}
