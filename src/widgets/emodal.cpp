#include "emodal.h"

#include "emainwindow.h"
#include "epainter.h"

#include <SDL2/SDL.h>

void eModal::initializeMask(const int w, const int h) {
    mMask = true;
    resize(window()->width(), window()->height());

    mFrame = new eFramedWidget(window());
    mFrame->setType(eFrameType::message);
    mFrame->resize(w, h);
    addWidget(mFrame);
    mFrame->align(eAlignment::center);
}

void eModal::resetModal() {
    removeChildren();
    mFrame = nullptr;
    mMask = false;
}

void eModal::close() {
    if(mCloseAction) mCloseAction();
    else deleteLater();
}

void eModal::paintEvent(ePainter& p) {
    if(mMask) {
        SDL_SetRenderDrawBlendMode(p.renderer(), SDL_BLENDMODE_BLEND);
        p.fillRect({0, 0, width(), height()}, {30, 30, 30, 180});
    } else {
        eFramedWidget::paintEvent(p);
    }
}

bool eModal::keyPressEvent(const eKeyPressEvent& e) {
    if(e.key() == SDL_SCANCODE_ESCAPE) {
        close();
        return true;
    }
    return false;
}

bool eModal::mousePressEvent(const eMouseEvent& e) {
    return true;
}

bool eModal::mouseReleaseEvent(const eMouseEvent& e) {
    if(e.button() == eMouseButton::right) {
        close();
        return true;
    }
    return true;
}
