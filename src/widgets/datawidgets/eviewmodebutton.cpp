#include "eviewmodebutton.h"

#include "textures/game-textures.h"

eViewModeButton::eViewModeButton(const std::string& text,
                                 const eViewMode vm,
                                 MainWindow* const window) :
    eCheckableButton(window), mVM(vm) {
    setCheckAction([this](const bool) {
        if(!mGW) return;
        const auto gwvm = mGW->viewMode();
        if(gwvm == mVM) {
            mGW->setViewMode(eViewMode::defaultView);
        } else {
            mGW->setViewMode(mVM);
        }
    });

    const auto& intrfc = GameTextures::interface();
    const int iRes = GameTextures::interfaceTextureId();
    const auto& texs = intrfc[iRes].fSeeButton;

    setTextureDrawScale(topSidebarTextureScale());
    setTexture(texs.getTexture(0));
    setHoverTexture(texs.getTexture(1));
    setCheckedTexture(texs.getTexture(2));
    setPaddingXXS();
    fitContent();

    const auto label = new eLabel(text, window);
    label->setFontSizeXS();
    label->setNoPadding();
    label->fitContent();
    addWidget(label);
    label->align(Alignment::center);
}

void eViewModeButton::setGameWidget(GameWidget* const gw) {
    mGW = gw;
}

void eViewModeButton::paintEvent(ePainter& p) {
    if(mGW) {
        const auto vm = mGW->viewMode();
        setChecked(vm == mVM);
    }
    eCheckableButton::paintEvent(p);
}
