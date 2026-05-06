#include "elabelbase.h"

#include "emainwindow.h"

#include <algorithm>

eLabelBase::eLabelBase(eMainWindow* const window) :
    mWindow(window) {

}

bool eLabelBase::setFont(const eFont& font) {
    const auto ttf = eFonts::requestFont(font);
    return setFont(ttf);
}

bool eLabelBase::setFontSizeXXS() {
    const int s = res().fontSizeXXS();
    return setFontSize(s);
}

bool eLabelBase::setFontSizeXS() {
    const int s = res().fontSizeXS();
    return setFontSize(s);
}

bool eLabelBase::setFontSizeS() {
    const int s = res().fontSizeS();
    return setFontSize(s);
}

bool eLabelBase::setFontSizeM() {
    const int s = res().fontSizeM();
    return setFontSize(s);
}

bool eLabelBase::setFontSizeL() {
    const int s = res().fontSizeL();
    return setFontSize(s);
}

bool eLabelBase::setFontSizeXL() {
    const int s = res().fontSizeXL();
    return setFontSize(s);
}

bool eLabelBase::setFontSize(const int s) {
    const auto font = eFonts::defaultFont(s);
    return setFont(font);
}

bool eLabelBase::setFont(TTF_Font* const font) {
    mFont = font;
    return updateTextTexture();
}

bool eLabelBase::updateTexture() {
    if(!mText.empty()) {
        updateTextTexture();
        return true;
    } else {
        return false;
    }
}

bool eLabelBase::setText(const std::string& text) {
    if(text == mText) return true;
    mText = text;
    return updateTextTexture();
}

bool eLabelBase::setTexture(const std::shared_ptr<eTexture>& tex) {
    mTexture = tex;
    return true;
}

bool eLabelBase::setFontColor(const eFontColor color) {
    mFontColor = color;
    return updateTextTexture();
}

void eLabelBase::setLightFontColor() {
    setFontColor(eFontColor::light);
}

void eLabelBase::setDarkFontColor() {
    setFontColor(eFontColor::dark);
}

void eLabelBase::setYellowFontColor() {
    setFontColor(eFontColor::yellow);
}

void eLabelBase::setTextureColorMod(const Uint8 r, const Uint8 g, const Uint8 b) {
    mR = r;
    mG = g;
    mB = b;
}

int eLabelBase::fontSize() const {
    if(!mFont) return 0;
    return TTF_FontHeight(mFont);
}

void eLabelBase::setWrapWidth(const int w) {
    mWidth = w;
    updateTextTexture();
}

bool eLabelBase::updateTextTexture() {
    if(mText.empty()) {
        mTexture.reset();
        return true;
    }
    if(!mFont) return false;
    mTexture = std::make_shared<eTexture>();
    const auto r = mWindow->renderer();
    const bool v = mTexture->loadText(r, mText, mFontColor, *mFont,
                                      mWidth, mWrapAlign);
    if(!v) {
        mTexture.reset();
        mUpdateTextTextureFailed = true;
    }
    return true;
}

const eResolution& eLabelBase::res() const {
    return mWindow->resolution();
}

int eLabelBase::lineWidth() const {
    return std::max(1, fontSize()/15);
}

void eLabelBase::textureSize(int& w, int& h) const {
    if(!mTexture) {
        w = 0;
        h = 0;
    } else {
        w = mTexture->width();
        h = mTexture->height();
    }
}

const std::shared_ptr<eTexture>& eLabelBase::texture() {
    if(mUpdateTextTextureFailed) {
        mUpdateTextTextureFailed = false;
        updateTextTexture();
    }
    return mTexture;
}
