#ifndef EARMYMENU_H
#define EARMYMENU_H

#include "elabel.h"

class eMiniMap;
class eGameBoard;
class eBasicButton;
class GameWidget;
class SoldierBanner;

class eArmyMenu : public eLabel {
public:
    using eLabel::eLabel;

    void initialize(eGameBoard& b);
    void setGameWidget(GameWidget* const gw) { mGW = gw; }

    eMiniMap* miniMap() const { return mMiniMap; }
    void setSoldiersHome(const bool h);
private:
    std::vector<SoldierBanner*> selectedPlayerBanners() const;

    eGameBoard* mBoard = nullptr;
    eMiniMap* mMiniMap = nullptr;
    eBasicButton* mGoToBanner = nullptr;
    eBasicButton* mGoHome = nullptr;
    GameWidget* mGW = nullptr;
};

#endif // EARMYMENU_H