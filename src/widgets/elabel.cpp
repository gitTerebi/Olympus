#include "elabel.h"

eLabel::eLabel(MainWindow* const window) :
    eWidget(window), eLabelBase(window) {
    setFont(eFonts::defaultFont(resolution()));
}

eLabel::eLabel(const std::string& text,
               MainWindow* const window) :
    eLabel(window) {
    setText(text);
}

void eLabel::fitOptions(const std::vector<std::string> &options) {
    int w = 0;
    const auto tmp = text();
    for(const auto& v : options) {
        setText(v);
        fitContent();
        const int wv = width();
        if(wv > w) w = wv;
    }
    setWidth(w);
    setText(tmp);
}

void eLabel::renderTargetsReset() {
    eWidget::renderTargetsReset();
    updateTexture();
}

void eLabel::sizeHint(int& w, int& h) {
    const auto& tex = texture();
    if(tex) {
        w = scaledTextureWidth(tex);
        h = scaledTextureHeight(tex);
    } else {
        eWidget::sizeHint(w, h);
    }
}

void eLabel::paintEvent(ePainter& p) {
    const auto& tex = texture();
    if(tex) {
        paintTexture(p, tex, textAlignment());
    }
}

void eLabel::paintTexture(ePainter& p,
                          const std::shared_ptr<Texture>& tex,
                          const Alignment align) {
    if(!tex) return;
    if(mTextureDrawScale == 1.0) {
        if(mR != 255 || mG != 255 || mB != 255) {
            tex->setColorMod(mR, mG, mB);
        }
        p.drawTexture(rect(), tex, align);
        if(mR != 255 || mG != 255 || mB != 255) {
            tex->clearColorMod();
        }
        return;
    }

    const int drawW = std::round(tex->width()*mTextureDrawScale) +
                      mTextureDrawBleed;
    const int drawH = std::round(tex->height()*mTextureDrawScale) +
                      mTextureDrawBleed;
    int x;
    if(static_cast<bool>(align & Alignment::right)) {
        x = width() - drawW;
    } else if(static_cast<bool>(align & Alignment::hcenter)) {
        x = (width() - drawW)/2;
    } else {
        x = 0;
    }
    int y;
    if(static_cast<bool>(align & Alignment::bottom)) {
        y = height() - drawH;
    } else if(static_cast<bool>(align & Alignment::vcenter)) {
        y = (height() - drawH)/2;
    } else {
        y = 0;
    }
    const SDL_Rect srcRect{tex->x(), tex->y(), tex->width(), tex->height()};
    const SDL_Rect dstRect{p.x() + x, p.y() + y, drawW, drawH};
    if(mR != 255 || mG != 255 || mB != 255) {
        tex->setColorMod(mR, mG, mB);
    }
    tex->render(p.renderer(), srcRect, dstRect);
    if(mR != 255 || mG != 255 || mB != 255) {
        tex->clearColorMod();
    }
}

int eLabel::scaledTextureWidth(const std::shared_ptr<Texture>& tex) const {
    if(!tex) return 0;
    return std::round(tex->width()*mTextureDrawScale);
}

int eLabel::scaledTextureHeight(const std::shared_ptr<Texture>& tex) const {
    if(!tex) return 0;
    return std::round(tex->height()*mTextureDrawScale);
}
