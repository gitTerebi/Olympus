#include "emainmenu.h"

#include <algorithm>

#include "ebuttonutils.h"
#include "enamewidget.h"
#include "elanguage.h"
#include "emainwindow.h"
#include "engine/world-city.h"


void eMainMenu::initialize(const eAction& continueGameA,
                            const bool continueGameEnabled,
                            const eAction& newGameA,
                            const eAction& loadGameA,
                            const eAction& editGameA,
                            const eAction& settingsA,
                            const eAction& optionsA,
                            const eAction& quitA,
                            const eAction& leaderA) {
    eMainMenuBase::initialize();

    const auto w = window();

    const auto buttons = new eWidget(w);
    addWidget(buttons);

    const auto res = resolution();
    const int cww = res.centralWidgetLargeWidth();
    const int cwh = res.centralWidgetLargeHeight();
    buttons->resize(cww, cwh);

    buttons->align(eAlignment::center);

    const auto continueGame = addPlainButton("Continue Adventure",
                                             continueGameA, buttons, w);
    continueGame->setEnabled(continueGameEnabled);
    addFramedButton(eLanguage::zeusText(1, 1), newGameA, buttons, w);
    addPlainButton(eLanguage::zeusText(1, 3), loadGameA, buttons, w);
    addPlainButton(eLanguage::zeusText(287, 3), editGameA, buttons, w);
    addPlainButton("Graphics", settingsA, buttons, w);
    addPlainButton("Options", optionsA, buttons, w);
    addPlainButton(eLanguage::zeusText(1, 5), quitA, buttons, w);

    buttons->layoutVertically();

    const auto leader = new FramedButton(w);
    leader->setRenderBg(true);
    leader->setUnderline(false);
    leader->setPressAction(leaderA);
    leader->setText(w->leader());
    leader->fitContent();
    addWidget(leader);
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
    leader->setX(imgX + 2*p);
    leader->setY(2*p);
}
