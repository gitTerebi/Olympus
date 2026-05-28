#ifndef EDATAWIDGET_H
#define EDATAWIDGET_H

#include "../ewidget.h"
#include "engine/ecityid.h"

class eViewModeButton;
class GameWidget;
class eGameBoard;
class eBasicButton;
enum class eCityId;

class eDataWidget : public eWidget {
public:
    eDataWidget(eGameBoard& b, eMainWindow* const w);

    virtual void initialize();

    void setGameWidget(GameWidget* const gw);
    virtual void shown();
    void update();
protected:
    virtual void openMoreInfoWiget() {}

    void setLastPersonCityId(const eCityId id) { mLastPersonCityId = id; }
    eCityId lastPersonCityId() const { return mLastPersonCityId; }

    void addViewButton(eViewModeButton* const b);
    eWidget* innerWidget() const { return mInnerWidget; }
    GameWidget* gameWidget() const { return mGW; }
    int spacing() const;
    void showMoreInfoButton();

    static int sCoverageToText(const int c);

    eCityId viewedCity();

    eGameBoard& mBoard;
    int mTime = 0;
private:
    eCityId mLastPersonCityId = eCityId::neutralFriendly;
    GameWidget* mGW = nullptr;
    eBasicButton* mMoreInfo = nullptr;
    eWidget* mInnerWidget = nullptr;
    std::vector<eViewModeButton*> mButtons;
};

#endif // EDATAWIDGET_H
