#include "eflatbutton.h"

#include <algorithm>
#include <cmath>

#include "textures/game-textures.h"

void eFlatButton::sizeHint(int &w, int &h) {
    eButtonBase::sizeHint(w, h);
    int iRes;
    double mult;
    iResAndMult(iRes, mult);
    const double s = mult/2.0;
    const int dim = GameTextures::interfaceTileDim();
    h = std::round(dim*s);
    w += std::round(4*dim*s);
    const auto& intrfc = GameTextures::interface()[iRes];
    if(intrfc.fLoaded) {
        const auto tex = intrfc.fBuildingButton.getTexture(0);
        if(tex) {
            const int th = std::round(tex->height()*s);
            if(th > h) h = th;
        }
    }
}

void eFlatButton::paintEvent(ePainter& p) {
    int iRes;
    double mult;
    iResAndMult(iRes, mult);
    const auto& intrfc = GameTextures::interface()[iRes];
    if(!intrfc.fLoaded) return;

    const TextureCollection& coll =
            hovered() ? intrfc.fBuildingButtonHover :
                        intrfc.fBuildingButton;

    const auto tex0 = coll.getTexture(0);
    if(!tex0 || tex0->height() <= 0) return;

    const int h = height();
    const double s = double(h)/tex0->height();
    const int dim = std::max(1, static_cast<int>(
        std::round(GameTextures::interfaceTileDim()*s)));

    const int iMax = width()/dim + 1;
    const int lastX = width() - dim;

    const auto drawTex = [&](const int x, const stdsptr<Texture>& tex) {
        if(!tex) return;
        const SDL_Rect srcRect{tex->x(), tex->y(),
                               tex->width(), tex->height()};
        const SDL_Rect dstRect{p.x() + x, p.y(), dim, h};
        tex->render(p.renderer(), srcRect, dstRect);
    };

    for(int i = 0; i < iMax; i++) {
        const int x = i == iMax - 1 ? lastX : dim*i;
        stdsptr<Texture> tex;
        if(i == 0) {
            tex = coll.getTexture(0);
        } else if(i == iMax - 1) {
            tex = coll.getTexture(2);
        } else {
            tex = coll.getTexture(1);
        }
        drawTex(x, tex);
    }

    eButtonBase::paintEvent(p);
}
