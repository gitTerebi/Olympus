#include "game-menu-base.h"

#include "datawidgets/edatawidget.h"

#include <cmath>

void GameMenuBase::initialize() {
    const double textureScale = topSidebarTextureScale();
    const int x = std::round(6*textureScale);
    const int y = std::round(22*textureScale);
    mButtonsWidget = new eWidget(window());
    mButtonsWidget->setX(x);
    mButtonsWidget->setY(y);
    mButtonsWidget->setPadding(0);
    addWidget(mButtonsWidget);
}

void GameMenuBase::clearMenuBaseState()
{
    mButtonsWidget = nullptr;
    mButtons.clear();
    mWidgets.clear();
}

eCheckableButton* GameMenuBase::addButton(
        const TextureCollection& texs,
        const eWid& w) {
    const auto b = eCheckableButton::sCreate(texs, window(), mButtonsWidget);
    b->setTextureDrawScale(topSidebarTextureScale());
    b->setTextureDrawBleed(topSidebarTextureBleed());
    b->fitContent();
    mButtons.push_back(b);
    mWidgets.push_back(w);
    return b;
}

void GameMenuBase::connectAndLayoutButtons() {
    connectButtons();
    layoutButtons();
}

void GameMenuBase::layoutButtons() {
    const int n = mButtons.size();
    const double textureScale = topSidebarTextureScale();
    const int originY = std::round(22*textureScale);
    for(int i = 0; i < n; i++) {
        const auto b = mButtons[i];
        b->setY(std::round(textureScale*(22 + i*41)) - originY);
    }
    const auto last = mButtons.back();
    mButtonsWidget->setHeight(last->y() + last->height());
    mButtonsWidget->fitContent();
}

void GameMenuBase::connectButtons() {
    const int iMax = mButtons.size();
    for(int i = 0; i < iMax; i++) {
        const auto b = mButtons[i];
        b->setCheckAction([this, i, b](const bool c) {
            if(c) {
                const int jMax = mButtons.size();
                const int wSize = mWidgets.size();
                for(int j = 0; j < jMax; j++) {
                    if(j < wSize) {
                        const auto w = mWidgets[j];
                        w.fW->setVisible(j == i);
                        if(j == i && w.fDW) {
                            w.fDW->shown();
                        }
                    }
                    if(j == i) continue;
                    const auto b = mButtons[j];
                    b->setChecked(false);
                }
            } else {
                b->setChecked(true);
            }
        });
    }
}

void GameMenuBase::selectTab(const int i) {
    if(i < 0 || i >= (int)mButtons.size()) return;
    mButtons[i]->check();
}

bool GameMenuBase::mousePressEvent(const eMouseEvent& e) {
    (void)e;
    return true;
}

bool GameMenuBase::mouseReleaseEvent(const eMouseEvent& e) {
    (void)e;
    return true;
}

bool GameMenuBase::mouseMoveEvent(const eMouseEvent& e) {
    (void)e;
    return false;
}

bool GameMenuBase::mouseEnterEvent(const eMouseEvent& e) {
    (void)e;
    return true;
}

bool GameMenuBase::mouseLeaveEvent(const eMouseEvent& e) {
    (void)e;
    return true;
}
