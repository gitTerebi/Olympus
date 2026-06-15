#include "emainmenu.h"

#include <algorithm>

#include "ebuttonutils.h"
#include "enamewidget.h"
#include "language.h"
#include "main-window.h"
#include "engine/world-city.h"


void eMainMenu::initialize(const eAction& continueGameA,
                            const bool continueGameEnabled,
                            const eAction& newGameA,
                            const eAction& loadGameA,
                            const eAction& editGameA,
                            const eAction& optionsA,
                            const eAction& quitA,
                            const eAction& leaderA) {
    eMainMenuBase::initialize();

    const auto w = window();

    mButtons = new eWidget(w);
    addWidget(mButtons);

    const auto continueGame = addPlainButton("Continue Adventure", continueGameA,
                                             mButtons, w);
    continueGame->setEnabled(continueGameEnabled);
    addFramedButton(Language::zeusText(1, 1), newGameA, mButtons, w);
    addPlainButton(Language::zeusText(1, 3), loadGameA, mButtons, w);
    addPlainButton(Language::zeusText(287, 3), editGameA, mButtons, w);
    addPlainButton("Options", optionsA, mButtons, w);
    addPlainButton(Language::zeusText(1, 5), quitA, mButtons, w);

    mButtons->layoutVertically();

    mLeader = new FramedButton(w);
    mLeader->setRenderBg(true);
    mLeader->setUnderline(false);
    mLeader->setPressAction(leaderA);
    mLeader->setText(w->leader());
    mLeader->fitContent();
    addWidget(mLeader);

    layoutControls();
}

void eMainMenu::paintEvent(ePainter& p) {
    eMainMenuBase::paintEvent(p);
}

void eMainMenu::windowSizeChanged(const int w, const int h) {
    eMainMenuBase::windowSizeChanged(w, h);
    layoutControls();
}

void eMainMenu::layoutControls() {
    const auto res = resolution();
    if(mButtons) {
        mButtons->resize(res.centralWidgetLargeWidth(),
                         res.centralWidgetLargeHeight());
        mButtons->align(Alignment::center);
        mButtons->layoutVertically();
        for(const auto child : mButtons->children()) {
            child->align(Alignment::hcenter);
        }
    }

    if(!mLeader) return;
    mLeader->fitContent();
    const int p = res.paddingXL();
    int tw;
    int th;
    textureSize(tw, th);
    const int ww = width();
    const int wh = height();
    int imgX = 0;
    if(tw > 0 && th > 0) {
        const float scale = std::min((float)ww/tw, (float)wh/th);
        const int dw = (int)(tw * scale);
        const int dh = (int)(th * scale);
        imgX = (ww - dw) / 2;
    }
    mLeader->setX(imgX + 2*p);
    mLeader->setY(2*p);
}
