#include "emainmenubase.h"

#include "textures/egametextures.h"

void eMainMenuBase::initialize() {
    const auto& intrfc = eGameTextures::interface();
    const auto res = resolution();
    const int iRes = static_cast<int>(res.uiScale());
    const auto& texs = intrfc[iRes];
    setTexture(texs.fMainMenuImage);
}

#include <algorithm>

void eMainMenuBase::paintEvent(ePainter& p) {
    const auto& tex = texture();
    if(tex) {
        const int ww = width();
        const int wh = height();
        int tw, th;
        textureSize(tw, th);
        if(tw > 0 && th > 0) {
            const float scale = std::max((float)ww / tw, (float)wh / th);
            const int dw = (int)(tw * scale);
            const int dh = (int)(th * scale);
            const int x = (ww - dw) / 2 + p.x();
            const int y = (wh - dh) / 2 + p.y();
            const SDL_Rect srcRect{0, 0, tw, th};
            const SDL_Rect dstRect{x, y, dw, dh};
            tex->render(p.renderer(), srcRect, dstRect);
        }
    }
}
