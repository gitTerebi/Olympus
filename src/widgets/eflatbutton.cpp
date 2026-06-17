#include "eflatbutton.h"

#include "textures/game-textures.h"

void eFlatButton::sizeHint(int &w, int &h) {
    eButtonBase::sizeHint(w, h);
    int iRes;
    double mult;
    iResAndMult(iRes, mult);
    const int dim = GameTextures::interfaceTileDim();
    h = dim;
    w += 4*dim;
    const auto& intrfc = GameTextures::interface()[iRes];
    if(intrfc.fLoaded) {
        const auto tex = intrfc.fBuildingButton.getTexture(0);
        if(tex && tex->height() > h) h = tex->height();
    }
}

void eFlatButton::paintEvent(ePainter& p) {
    int iRes;
    double mult;
    iResAndMult(iRes, mult);
    const int dim = GameTextures::interfaceTileDim();
    const auto& intrfc = GameTextures::interface()[iRes];
    if(!intrfc.fLoaded) return;

    const int iMax = width()/dim + 1;
    const int lastX = width() - dim;

    const TextureCollection& coll =
            hovered() ? intrfc.fBuildingButtonHover :
                        intrfc.fBuildingButton;

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
        p.drawTexture(x, 0, tex);
    }

    eButtonBase::paintEvent(p);
}
