#include "etooltip.h"

#include "emainwindow.h"
#include "widgets/ewidget.h"

#include <SDL2/SDL.h>

eTooltip::eTooltip(eMainWindow& w) : mWindow(w) {}

void eTooltip::update() {
    const auto txt = eWidget::sTooltip();
    const bool updateTxt = mText != txt;
    if(updateTxt) {
        mText = txt;
    }

    const auto& res = mWindow.resolution();
    const int fontSize = res.verySmallFontSize();
    const bool updateFont = mFontSize != fontSize;
    if(updateFont) {
        mFontSize = fontSize;
        mFont = eFonts::defaultFont(mFontSize);
    }

    const bool updateTexture = updateTxt || updateFont;
    if(updateTexture) {
        const auto r = mWindow.renderer();
        if(mText.empty()) {
            mTexture->reset();
        } else {
            mTexture->loadText(r, mText, eFontColor::light, *mFont, 50*fontSize);
        }
    }
}

void eTooltip::paint(const int x, const int y, ePainter& p) {
    const int pp = padding();
    SDL_Rect rect{x, y, width(), height()};
    p.fillRect(rect, SDL_Color{16, 108, 144, 255});
    p.drawRect(rect, SDL_Color{0, 32, 32, 255}, 1);
    p.drawTexture(x + pp, y + pp, mTexture);
}

int eTooltip::width() const { return mTexture->width() + 2*padding(); }
int eTooltip::height() const { return mTexture->height() + 2*padding(); }

bool eTooltip::empty() const { return mText.empty(); }

int eTooltip::padding() const { return mFontSize/2; }