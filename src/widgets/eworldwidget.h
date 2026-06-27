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
    void setWorldBoard(WorldBoard* const board);

    void update();

    // dev: select a city as if clicked, so its status panel shows
    bool selectCityByName(const std::string& name);

    using eEnlistAction = eEnlistForcesDialog::eEnlistAction;
    void openEnlistForcesDialog(
        const eEnlistAction& a,
        const std::vector<stdsptr<WorldCity>>& exclude,
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
    void windowSizeChanged(int w, int h) override;
private:
    void layoutControls();
    void openRequestDialog();
    void openFulfillDialog();
    void openGiftDialog();
    void setMap(const eWorldMap map);

    stdsptr<WorldCity> mCity;
    eWorldMenu* mWM = nullptr;
    eWorldMapWidget* mWMW = nullptr;
    WorldBoard* mWorldBoard = nullptr;
    GameBoard* mBoard = nullptr;
    FramedButton* mMapButton = nullptr;
    FramedButton* mAddCityButton = nullptr;
    FramedButton* mSettingsButton = nullptr;
};

#endif // EWORLDWIDGET_H
