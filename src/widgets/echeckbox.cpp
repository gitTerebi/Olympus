#include "echeckbox.h"

#include "textures/game-textures.h"

eCheckBox::eCheckBox(MainWindow* const window) :
    eCheckableButton(window) {
    const auto res = resolution();
    const auto uiScale = res.uiScale();
    const int iRes = GameTextures::interfaceTextureId();
    const auto& intrfc = GameTextures::interface();
    const auto& texs = intrfc[iRes].fCheckBox;
    setCheckedTexture(texs.getTexture(0));
    setTexture(texs.getTexture(1));
    setCheckedHoverTexture(texs.getTexture(2));
    setHoverTexture(texs.getTexture(3));
    setPadding(0);
    fitContent();
}
