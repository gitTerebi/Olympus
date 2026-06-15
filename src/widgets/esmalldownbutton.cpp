#include "esmalldownbutton.h"

#include "textures/game-textures.h"

eSmallDownButton::eSmallDownButton(MainWindow* const window) :
    eButton(window) {
    const auto& intrfc = GameTextures::interface();
    const auto res = resolution();
    const auto uiScale = res.uiScale();
    const int iRes = static_cast<int>(uiScale);
    const auto& texs = intrfc[iRes];

    setPadding(0);
    const auto& coll = texs.fSmallDownButton;
    setTexture(coll.getTexture(0));
    setPressedTexture(coll.getTexture(1));
    fitContent();

    setUnderline(false);
}
