#include "ebasicbutton.h"

eBasicButton::eBasicButton(const eTex tex,
                           MainWindow* const window) :
    eButton(window) {

    const auto& intrfc = GameTextures::interface();
    const auto res = resolution();
    const auto uiScale = res.uiScale();
    const int iRes = GameTextures::interfaceTextureId();
    const auto& texs = intrfc[iRes];

    setPadding(0);
    const auto& coll = (texs.*tex);
    setTexture(coll.getTexture(0));
    setHoverTexture(coll.getTexture(1));
    setPressedTexture(coll.getTexture(2));
    fitContent();
}
