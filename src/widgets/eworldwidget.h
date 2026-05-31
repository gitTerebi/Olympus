#ifndef EWORLDWIDGET_H
#define EWORLDWIDGET_H

#include "emainwidget.h"
#include "engine/game-board.h"

#include "widgets/eenlistforcesdialog.h"

class eWorldMenu;
class eWorldMapWidget;
class FramedButton;

class eWorldWidget : public eMainWidget {
public:
    using eMainWidget::eMainWidget;

    void initialize();

    void setBoard(GameBoard* const board);
    void setWorldBoard(eWorldBoard* const board);

    void update();

    using eEnlistAction = eEnlistForcesDialog::eEnlistAction;
    void openEnlistForcesDialog(
        const eEnlistAction& a,
        const std::vector<stdsptr<eWorldCity>>& exclude,
        const std::vector<eResourceType>& plunderResources = {},
        const bool onlySoldiers = false);
    void openEnlistForcesDialog(const eEnlistedForces& enlistable,
            const std::vector<eCityId>& cids,
            const std::vector<std::string>& cnames,
            const std::vector<eHeroType>& heroesAbroad,
            const eEnlistAction& action,
            const std::vector<eResourceType>& plunderResources = {});

    void openDialog(eWidget* const d) override;
protected:
    bool keyPressEvent(const eKeyPressEvent& e) override;
private:
    void openRequestDialog();
    void openFulfillDialog();
    void openGiftDialog();
    void setMap(const eWorldMap map);

    stdsptr<eWorldCity> mCity;
    eWorldMenu* mWM = nullptr;
    eWorldMapWidget* mWMW = nullptr;
    eWorldBoard* mWorldBoard = nullptr;
    GameBoard* mBoard = nullptr;
    FramedButton* mMapButton = nullptr;
    FramedButton* mAddCityButton = nullptr;
    FramedButton* mSettingsButton = nullptr;
};

#endif // EWORLDWIDGET_H
