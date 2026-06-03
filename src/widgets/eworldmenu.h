#ifndef EWORLDMENU_H
#define EWORLDMENU_H

#include "elabel.h"

#include "ebutton.h"
#include "engine/world-city.h"
#include "pointers/estdselfref.h"

class eWorldGoodsWidget;
class eWorldTributeWidget;

class WorldBoard;
class GameBoard;

class eWorldMenu : public eLabel {
public:
    using eLabel::eLabel;

    void initialize(const eAction& openRequest,
                    const eAction& openFulfill,
                    const eAction& openGift,
                    const eAction& openRaid,
                    const eAction& openConquer,
                    const bool showText = true);

    void setCity(const stdsptr<WorldCity>& c);
    void setWorldBoard(WorldBoard* const b);
    void setGameBoard(GameBoard* const b);
    void setText(const std::string& text);
    void setArrowActions(const eAction& left, const eAction& right);
    void updateLabels() const;
    void updateButtonsEnabled() const;
private:
    WorldBoard* mBoard = nullptr;

    eLabel* mTextLabel = nullptr;

    eLabel* mRelationshipLabel = nullptr;
    eLabel* mNameLabel = nullptr;
    eLabel* mLeaderLabel = nullptr;

    eLabel* mAttitudeLabel = nullptr;

    eWorldGoodsWidget* mGoodsWidget = nullptr;
    eWorldTributeWidget* mTributeWidget = nullptr;

    eButton* mRequestButton = nullptr;
    eButton* mFulfillButton = nullptr;
    eButton* mGiftButton = nullptr;
    eButton* mRaidButton = nullptr;
    eButton* mConquerButton = nullptr;

    eButton* mLeftArrowButton = nullptr;
    eButton* mRightArrowButton = nullptr;

    stdsptr<WorldCity> mCity;

    bool mShowText = true;
};

#endif // EWORLDMENU_H
