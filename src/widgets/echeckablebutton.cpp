#include "echeckablebutton.h"

eCheckableButton::eCheckableButton(MainWindow* const window) :
    eButton(window) {
    setPressAction([this]() {
        mChecked = !mChecked;
    });
}

void eCheckableButton::setCheckAction(const eCheckAction& a) {
    mCheckAction = a;
    setPressAction([this]() {
        mChecked = !mChecked;
        if(mCheckAction) mCheckAction(mChecked);
    });
}

void eCheckableButton::setChecked(const bool c) {
    mChecked = c;
}

void eCheckableButton::check() {
    if(mChecked) return;
    mChecked = true;
    if(mCheckAction) mCheckAction(true);
}

void eCheckableButton::setCheckedTexture(const std::shared_ptr<Texture>& tex) {
    mCheckedTexture = tex;
}

void eCheckableButton::setCheckedHoverTexture(const std::shared_ptr<Texture>& tex) {
    mCheckedHoverTexture = tex;
}

eCheckableButton* eCheckableButton::sCreate(
        const TextureCollection& texs,
        MainWindow* const window,
        eWidget* const buttons) {
    const auto b = sCreateButtonBase<eCheckableButton>(texs, window, buttons);
    b->setCheckedTexture(texs.getTexture(2));
    return b;
}

void eCheckableButton::paintEvent(ePainter& p) {
    if(mCheckedTexture) {
        if(mChecked) {
            if(mCheckedHoverTexture && hovered()) {
                paintTexture(p, mCheckedHoverTexture, Alignment::center);
            } else {
                paintTexture(p, mCheckedTexture, Alignment::center);
            }
        } else eButton::paintEvent(p);
    } else {
        eButton::paintEvent(p);
        if(mChecked) {
            SDL_Color col1;
            SDL_Color col2;
            FontColorHelpers::colors(fontColor(), col1, col2);
            const SDL_Rect rect2{rect()};
            const SDL_Rect rect1{rect2.x + 1, rect2.y + 1,
                                 rect2.w, rect2.h};
            p.drawRect(rect2, col2, lineWidth());
            p.drawRect(rect1, col1, lineWidth());
        }
    }
}
