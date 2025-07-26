#include "escrollwidget.h"

#include <algorithm>

#include "widgets/esmallupbutton.h"
#include "widgets/esmalldownbutton.h"

void eScrollWidget::initializeButtons() {
    const auto up = new eSmallUpButton(window());
    addWidget(up);
    up->align(eAlignment::top | eAlignment::right);
    up->setPressAction([this]() {
        scrollUp();
    });
    const auto down = new eSmallDownButton(window());
    addWidget(down);
    down->align(eAlignment::bottom | eAlignment::right);
    down->setPressAction([this]() {
        scrollDown();
    });
    mUpButton = up;
    mDownButton = down;
    clampDY();
}

void eScrollWidget::setScrollArea(eWidget* const w) {
    setMouseReceiver(w);
    mScrollArea = w;
    clampDY();
}

void eScrollWidget::scrollUp() {
    mDy -= 35;
    clampDY();
}

void eScrollWidget::scrollDown() {
    mDy += 35;
    clampDY();
}

void eScrollWidget::scrollToTheTop() {
    mDy = 0;
    clampDY();
}

void eScrollWidget::paintEvent(ePainter& p) {
    //p.fillRect(rect(), {255, 0, 0, 255});
    if(mScrollArea) {
        const auto r = rect();
        p.setClipRect(&r);

        p.translate(0, -mDy);
        mScrollArea->paint(p);
        p.translate(0, mDy);

        p.setClipRect(nullptr);
    }
}

bool eScrollWidget::keyPressEvent(const eKeyPressEvent& e) {
    if(e.key() == SDL_SCANCODE_UP) {
        scrollUp();
        return true;
    } else if(e.key() == SDL_SCANCODE_DOWN) {
        scrollDown();
        return true;
    }
    if(!mScrollArea) return false;
    return mScrollArea->keyPress(e.translated(0, -mDy));
}

bool eScrollWidget::mouseWheelEvent(const eMouseWheelEvent& e) {
    mDy -= 10*e.dy();
    clampDY();
    return true;
}

void eScrollWidget::clampDY() {
    if(mScrollArea) {
        const int sh = mScrollArea->height();
        const int h = height();
        const int maxH = sh - h;
        if(h > sh) {
            mDy = 0;
        } else {
            mDy = std::clamp(mDy, 0, maxH);
        }
        setMouseReceiverDXDY(0, mDy);
        if(mUpButton) mUpButton->setVisible(mDy > 0);
        if(mDownButton) mDownButton->setVisible(mDy < maxH);
    } else {
        mDy = 0;
        if(mUpButton) mUpButton->hide();
        if(mDownButton) mDownButton->hide();
    }
}

void eScrollWidget::renderTargetsReset() {
    eWidget::renderTargetsReset();
    if(mScrollArea) mScrollArea->renderTargetsReset();
}
