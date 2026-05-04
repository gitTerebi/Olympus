#include "epopupwidget.h"

#include "eframedwidget.h"
#include "emouseevent.h"
#include "emainwindow.h"
#include "epainter.h"

void ePopupWidget::paintEvent(ePainter& p) {
    SDL_SetRenderDrawBlendMode(p.renderer(), SDL_BLENDMODE_BLEND);
    p.fillRect({0, 0, width(), height()}, {30, 30, 30, 180});
}

void ePopupWidget::initialize(const int w, const int h) {
    resize(window()->width(), window()->height());

    mFrame = new eFramedWidget(window());
    mFrame->setType(eFrameType::message);
    mFrame->resize(w, h);
    addWidget(mFrame);
    mFrame->align(eAlignment::center);
}

void ePopupWidget::closePopup() {
    if(mCloseAction) mCloseAction();
    else deleteLater();
}

bool ePopupWidget::keyPressEvent(const eKeyPressEvent& e) {
    if(e.key() == SDL_SCANCODE_ESCAPE) {
        closePopup();
        return true;
    }
    return false;
}

bool ePopupWidget::mouseReleaseEvent(const eMouseEvent& e) {
    if(e.button() == eMouseButton::right) {
        closePopup();
        return true;
    }
    return false;
}
