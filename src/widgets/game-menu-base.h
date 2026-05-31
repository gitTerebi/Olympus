#ifndef GAME_MENU_BASE_H
#define GAME_MENU_BASE_H

#include "elabel.h"

#include "echeckablebutton.h"
#include "etexturecollection.h"

class eDataWidget;

struct eWid {
    eWid(eWidget* const w) :
        fW(w) {}
    eWid(eWidget* const w, eDataWidget* const dw) :
        fW(w), fDW(dw) {}

    eWidget* fW;
    eDataWidget* fDW = nullptr;
};

class GameMenuBase : public eLabel {
public:
    using eLabel::eLabel;

    void initialize();
    eCheckableButton* addButton(const eTextureCollection& coll,
                                const eWid& w);
    void connectAndLayoutButtons();
    void layoutButtons();
    void connectButtons();
    void selectTab(int i);
protected:
    bool mousePressEvent(const eMouseEvent& e) override;
    bool mouseReleaseEvent(const eMouseEvent& e) override;
    bool mouseMoveEvent(const eMouseEvent& e) override;
    bool mouseEnterEvent(const eMouseEvent& e) override;
    bool mouseLeaveEvent(const eMouseEvent& e) override;
private:
    eWidget* mButtonsWidget = nullptr;
    std::vector<eCheckableButton*> mButtons;
    std::vector<eWid> mWidgets;
};

#endif // GAME_MENU_BASE_H
