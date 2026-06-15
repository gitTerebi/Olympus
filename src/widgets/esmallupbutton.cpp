#include "esmallupbutton.h"

#include "textures/game-textures.h"

eSmallUpButton::eSmallUpButton(eMainWindow* const window) :
    eButton(window) {
    const auto& intrfc = GameTextures::interface();
    const auto res = resolution();
    const auto uiScale = res.uiScale();
    const int iRes = static_cast<int>(uiScale);
    const auto& texs = intrfc[iRes];

    setPadding(0);
    const auto& coll = texs.fSmallUpButton;
    setTexture(coll.getTexture(0));
    setPressedTexture(coll.getTexture(1));
    fitContent();

    setUnderline(false);
}
