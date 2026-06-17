#include "emicrobutton.h"

#include "textures/game-textures.h"

void eMicroButton::sizeHint(int& w, int& h) {
    eButtonBase::sizeHint(w, h);
    const auto res = resolution();
    const double mult = hasTextureDrawScale() ?
                            textureDrawScale() :
                            res.multiplier();
    h = std::round(13*mult);
}

void eMicroButton::paintEvent(ePainter& p) {
    const auto res = resolution();
    const int iRes = GameTextures::interfaceTextureId();
    const double mult = hasTextureDrawScale() ?
                            textureDrawScale() :
                            res.multiplier();
    const int wdim = std::round(18*mult);
    const auto& intrfc = GameTextures::interface()[iRes];
    if(!intrfc.fLoaded) return;

    const int iMax = width()/wdim + 1;
    const int lastX = width() - wdim;

    const bool h = hovered();
    const bool pp = pressed();
    const bool e = enabled();
    int texId;
    if(!e) texId = 3;
    else if(pp) texId = 2;
    else if(h) texId = 1;
    else texId = 0;

    for(int i = 0; i < iMax; i++) {
        const int x = i == iMax - 1 ? lastX : wdim*i;
        const TextureCollection* coll;
        if(i == 0) {
            coll = &intrfc.fMicroButton[0];
        } else if(i == iMax - 1) {
            coll = &intrfc.fMicroButton[2];
        } else {
            coll = &intrfc.fMicroButton[1];
        }
        const auto& tex = coll->getTexture(texId);
        if(!tex) continue;
        const int drawW = std::round(tex->width()*mult);
        const int drawH = std::round(tex->height()*mult);
        const SDL_Rect srcRect{tex->x(), tex->y(),
                               tex->width(), tex->height()};
        const SDL_Rect dstRect{p.x() + x, p.y(),
                               drawW, drawH};
        tex->render(p.renderer(), srcRect, dstRect);
    }

    eButtonBase::paintEvent(p);
}
