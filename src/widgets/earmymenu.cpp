#include "earmymenu.h"

#include "language.h"
#include "textures/game-textures.h"

#include "eminimap.h"
#include "ebasicbutton.h"
#include "eframedwidget.h"

#include "game-widget.h"
#include "characters/soldier-banner.h"

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
    int mult;
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
    wid->move(mult * 11.5, mult * 13);
    wid->resize(mult * 71, mult * 216);
    addWidget(wid);

    mMiniMap = new eMiniMap(window());
    wid->addWidget(mMiniMap);
    mMiniMap->resize(wid->width(), mult * 56);
    mMiniMap->setBoard(&b);

    const auto status = new eLabel(window());
    status->setNoPadding();
    status->setTextureDrawScale(topSidebarTextureScale());
    status->setTexture(coll.fArmyStatus);
    status->fitContent();
    wid->addWidget(status);
    status->resize(wid->width(), mult * 23);
    status->setX(mult * 0.75);
    status->setY(mult * 60.75);

    const int xx = mult * 37.75;
    const int dy = mult * 2.5;
    int y = mult * 88.5;

    const auto t1 = &InterfaceTextures::fGoToCompany;
    const auto cou = new eBasicButton(t1, window());
    cou->setTextureDrawScale(topSidebarTextureScale());
    cou->fitContent();
    wid->addWidget(cou);
    cou->setY(y);
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
    dt->setTextureDrawScale(topSidebarTextureScale());
    dt->fitContent();
    wid->addWidget(dt);
    dt->setY(y);
    dt->setX(xx);
    dt->setTooltip(Language::zeusText(51, 73)); // Defensive Tactics

    const int ddy = dy + cou->height();

    const auto t3 = &InterfaceTextures::fRotateCompany;
    const auto rc = new eBasicButton(t3, window());
    rc->setTextureDrawScale(topSidebarTextureScale());
    rc->fitContent();
    wid->addWidget(rc);
    rc->setY(y + ddy);
    rc->setTooltip(Language::zeusText(51, 76)); // Rotate Company
    rc->setPressAction([this]()
                       {
        if (!mBoard || !mGW) return;
        SoldierBanner::sRotatePlayerBanners(
            mBoard->selectedSoldiers(), mBoard->personPlayer()); });

    const auto t4 = &InterfaceTextures::fOffensiveTactics;
    const auto ot = new eBasicButton(t4, window());
    ot->setTextureDrawScale(topSidebarTextureScale());
    ot->fitContent();
    wid->addWidget(ot);
    ot->setY(y + ddy);
    ot->setX(xx);
    ot->setTooltip(Language::zeusText(51, 74)); // Offensive Tactics

    const auto t5 = &InterfaceTextures::fGoToBanner;
    mGoToBanner = new eBasicButton(t5, window());
    mGoToBanner->setTextureDrawScale(topSidebarTextureScale());
    mGoToBanner->fitContent();
    wid->addWidget(mGoToBanner);
    mGoToBanner->setY(y + 2 * ddy);
    mGoToBanner->setTooltip(Language::zeusText(51, 25)); // Go To Banner (Muster)
    mGoToBanner->setPressAction([this]()
                                {
        mBoard->bannersBackFromHome();
        setSoldiersHome(false); });

    const auto t5_2 = &InterfaceTextures::fGoHome;
    mGoHome = new eBasicButton(t5_2, window());
    mGoHome->setTextureDrawScale(topSidebarTextureScale());
    mGoHome->fitContent();
    wid->addWidget(mGoHome);
    mGoHome->setY(y + 2 * ddy);
    mGoHome->setTooltip(Language::zeusText(51, 26)); // Go Home
    mGoHome->hide();
    mGoHome->setPressAction([this]()
                            {
        mBoard->bannersGoHome();
        setSoldiersHome(true); });

    const auto t6 = &InterfaceTextures::fSpecialTactics;
    const auto st = new eBasicButton(t6, window());
    st->setTextureDrawScale(topSidebarTextureScale());
    st->fitContent();
    wid->addWidget(st);
    st->setY(y + 2 * ddy);
    st->setX(xx);
    st->setTooltip(Language::zeusText(51, 75)); // Special Tactics

    const auto ww = new eFramedWidget(window());
    wid->addWidget(ww);
    ww->setType(eFrameType::inner);
    ww->setY(y + 3 * ddy + mult * 2);
    ww->resize(wid->width(), mult * 50);
}

void eArmyMenu::setSoldiersHome(const bool h)
{
    mGoToBanner->setVisible(h);
    mGoHome->setVisible(!h);
}
